
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "socket_udp.h"
#include "CUtil.h"

int SocketUdp::initUdp(const char *localIp, uint16_t lockPort, const char *remoteIp, uint16_t remotePort)
{
    RETV_CHECK_VALUE((!localIp || !remoteIp), -1, "param null");
    int socketFd = -1;
    int ret = -1;
    struct timeval timeout;
	timeout.tv_sec = UDP_SEND_RECV_MSG_TIMEOUT;
	timeout.tv_usec = 0;

    LOG_INFO("%s:localIp=%s, lockPort=%d, remoteIp=%s, remotePort=%d\n", __func__, localIp, lockPort, remoteIp, remotePort);
	socketFd = socket(AF_INET,  SOCK_DGRAM,  0);
    RETV_CHECK_VALUE(socketFd < 0, -1, "Create socke failed");
    mUdpSocketInfo.socketFd = socketFd;
    memset(&mUdpSocketInfo.remoteAddr,  0,  sizeof(mUdpSocketInfo.remoteAddr));
	mUdpSocketInfo.remoteAddr.sin_family = AF_INET;
	mUdpSocketInfo.remoteAddr.sin_port = htons(remotePort);
	mUdpSocketInfo.remoteAddr.sin_addr.s_addr = inet_addr(remoteIp);
	memset(&mUdpSocketInfo.localAddr,  0,  sizeof(mUdpSocketInfo.localAddr));
	mUdpSocketInfo.localAddr.sin_family = AF_INET;
	mUdpSocketInfo.localAddr.sin_port = htons(lockPort);
	mUdpSocketInfo.localAddr.sin_addr.s_addr = inet_addr(localIp);

	ret = bind(socketFd,  (struct sockaddr*)&mUdpSocketInfo.localAddr,  sizeof(mUdpSocketInfo.localAddr));
    RETV_CHECK_VALUE(ret < 0, -1, "bind socket failed");
    ret = setsockopt(socketFd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
    RETV_CHECK_VALUE(ret < 0, -1, "setsockopt socket failed");
	ret = setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    RETV_CHECK_VALUE(ret < 0, -1, "setsockopt socket failed");
    LOG_INFO("udp socket Init ok\n");
    return socketFd;
}

int SocketUdp::SendUdpMessage(unsigned char *data, int len)
{
    int ret = -1;
    RETV_CHECK_VALUE(!data, -1, "para null");
    if(mUdpSocketInfo.debug)
    {
        LOG_INFO("%s:len=%d\n", __func__, len);
        LOG_INFO("------------------send start-----------------\n");
        int i=0;
        for(i=0;i<len;i++)
        {
            LOG_INFO("%02X ", data[i]);
        }
        LOG_INFO("\n");
        LOG_INFO("------------------send end-----------------\n");
    }
    ret = TEMP_FAILURE_RETRY_N_TIMES(sendto(mUdpSocketInfo.socketFd,  data,  len,  0,  (struct sockaddr*)&mUdpSocketInfo.remoteAddr,  sizeof(mUdpSocketInfo.remoteAddr)), 2);
    RETV_CHECK_VALUE(ret < 0,  -1, "sendto fist fail, ret=%d, error:%s(errno:%d), try again\n", ret, strerror(errno), errno);
    /*if(ret != len)
    {
        LOG_INFO("%s:sendto ret(%d) is not len(%d)\n", __func__, ret, len);
        return -1;
    }*/

    return ret;
}

int SocketUdp::receiveUdpMessage(unsigned char * recvBuf, int recvLen)
{
    int ret = -1;
    RETV_CHECK_VALUE(!recvBuf, -1, "para null");
    socklen_t tmpLen = sizeof(mUdpSocketInfo.remoteAddr);
	ret = TEMP_FAILURE_RETRY_N_TIMES(recvfrom(mUdpSocketInfo.socketFd,  recvBuf,  recvLen ,  0,  (struct sockaddr*)&mUdpSocketInfo.remoteAddr,  &tmpLen), 2);
    RETV_CHECK_VALUE(ret < 0,  -1, "recvfromfail, ret=%d, error:%s(errno:%d), try again\n", ret, strerror(errno), errno);
    if(mUdpSocketInfo.debug)
    {
        LOG_INFO("%s:ret=%d\n", __func__, ret);
        LOG_INFO("------------------recv start-----------------\n");
        int i=0;
        for(i=0;i<ret;i++)
        {
            LOG_INFO("%02X ", recvBuf[i]);
        }
        LOG_INFO("\n");
        LOG_INFO("------------------recv end-----------------\n");
    }
	/*if(ret != recvLen)
    {
        LOG_INFO("%s:recv ret(%d) is not recvLen(%d)\n", __func__, ret, recvLen);
        return -1;
    }*/
    return ret;
}

int SocketUdp::closeUdp()
{
    RETV_CHECK_VALUE(mUdpSocketInfo.socketFd <= 0, -1, "socketFd is not created");
    close(mUdpSocketInfo.socketFd);
    return 0;
}
