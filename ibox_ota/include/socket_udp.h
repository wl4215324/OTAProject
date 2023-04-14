#ifndef __SOCKET_UDP_H_
#define __SOCKET_UDP_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>



//#define MCU_DEBUG     1
#define UDP_SEND_RECV_MSG_TIMEOUT           5



class SocketUdp {
public:
    int initUdp(const char *localIp, uint16_t localPort, const char *remoteIp, uint16_t remotePort);
    int SendUdpMessage(unsigned char *data, int len);
    int receiveUdpMessage(unsigned char * recv_buf, int recv_len);
    int closeUdp();
private:

    using UDP_SOCKET_INFO_S =  struct
    {
        char localIp[16];
        uint16_t localPort;
        char remoteIp[16];
        uint16_t remotePort;
        struct sockaddr_in remoteAddr;
        struct sockaddr_in localAddr;
        int socketFd;
        int debug;
    } ;

    UDP_SOCKET_INFO_S mUdpSocketInfo;
};

#endif