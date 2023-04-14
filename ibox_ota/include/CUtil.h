#ifndef       _CUTIL_H_
#define    __CUTIL_H_

#include <unistd.h>


#define  global_log_level  (3)

#define LOG_ERR(fmt, args...)   \
            if (global_log_level >= 0) \
                printf("[ERR] %s-%s-%d: " fmt, __FILE__, __FUNCTION__, __LINE__, ##args);\
                printf("\n");

#define LOG_INFO(fmt, args...)   \
            if (global_log_level >= 2) \
                printf("[INFO] %s-%s-%d: " fmt, __FILE__, __FUNCTION__, __LINE__, ##args);\
                printf("\n");

#define LOG_DEBUG(fmt, args...)   \
            if (global_log_level >= 3) \
                printf("[DEBUG] %s-%s-%d: " fmt, __FILE__, __FUNCTION__, __LINE__, ##args);\
                printf("\n");

#define LOG_WARN(fmt, args...)   \
            if (global_log_level >= 1) \
                printf("[WARN] %s-%s-%d: " fmt, __FILE__, __FUNCTION__, __LINE__, ##args);\
                printf("\n");

#define TEMP_FAILURE_RETRY_N_TIMES(expression, n) \
    (__extension__({ long int result;						      \
        int count = n;\
       do result = (long int) (expression);				      \
       while ((result != 0L) && (--count < 0));			      \
       result; }))

#define  RET_CHECK_VALUE(status,  fmt, args...) \
    do {\
        if (status) {\
            printf("[ERR] %s-%d: " fmt, __FUNCTION__, __LINE__, ##args);\
            printf("\n");\
            return;\
        }\
    } while(false);

#define  RETV_CHECK_VALUE(status,  value,  fmt, args...) \
    do {\
        if (status) {\
            printf("[ERR] %s-%s-%d: " fmt, __FILE__, __FUNCTION__, __LINE__, ##args);\
            printf("\n");\
            return value;\
        }\
    } while(false);

#define EXE_SHELL_COMMAND(command)\
({\
    bool isSuccess = false;\
    do {\
        int32_t status = -1;\
        status = system(command);\
        if(status == -1){\
                LOG_ERR("run command %s failed", command);\
                isSuccess = false;\
        } else {\
            if(WIFEXITED(status)) {\
                if(WEXITSTATUS(status) == 0){\
                    isSuccess = true;\
                    LOG_INFO("run command %s success", command);\
                } else {\
                    isSuccess = false;\
                    LOG_ERR("run command %s fail and exit code is %d", command, WEXITSTATUS(status));\
                }\
            } else {\
                isSuccess = false;\
                LOG_ERR("exit status = %d\n",WEXITSTATUS(status));\
            }\
        }\
    } while(false);\
    isSuccess;\
})
#endif //__CUTIL_H_