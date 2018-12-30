#ifndef LOGGER_H
#define LOGGER_H

#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#define DIRECT_LOG(fmt, ...) printf("[DEBUG] %s(): "fmt"\n", __FUNCTION__, ##__VA_ARGS__);

#define LOG_INFO     1
#define LOG_WARN     1
#define LOG_ERROR    1
#define LOG_DEBUG    1
#define LOG_TRACE    1

//#define LOG(X)       if (LOG_##X)

/*
#define LOG_PRINT_FORMAT(log_type, format, args...) \
    if (log_type) \
        printf("%s %s ["#log_type"] %s():%d "format, __DATE__, __TIME__, __FUNCTION__, __LINE__, args);

#define LOG_PRINT_MSG(log_type, msg)                \
    if (log_type) \
        printf("%s %s ["#log_type"] %s():%d "msg, __DATE__, __TIME__, __FUNCTION__, __LINE__);

*/

#define LOG_DECLARE()                               \
    time_t cur_tm;                                  \
    pid_t currentPID = getpid();                    \
    pthread_t currentTID = pthread_self();          \
    FILE* fp_log;


#define LOG_PRINT_FORMAT(log_type, format, args...) \
    if( log_type )                                  \
    {                                               \
        time(&cur_tm);                              \
        printf("%.24s ["#log_type"] [PID=%d] [TID=%ld] %s():%d "format, ctime(&cur_tm), currentPID, currentTID, __FUNCTION__, __LINE__, args); \
    }
//printf("%.24s ["#log_type"] [PID=%d] [TID=%p] %s():%d "format, ctime(&cur_tm), currentPID, (void*)currentTID, __FUNCTION__, __LINE__, args);
	
#define LOG_PRINT_MSG(log_type, msg)                \
    if( log_type )                                  \
    {                                               \
        time(&cur_tm);                              \
        printf("%.24s ["#log_type"] [PID=%d] [TID=%ld] %s():%d "msg, ctime(&cur_tm), currentPID, currentTID, __FUNCTION__, __LINE__); \
    }

#define LOG_SAVE_FILE                               "log_saved"
#define LOG_SAVE_FORMAT(log_type, format, args...)  \
    if( log_type )                                  \
    {                                               \
        fp_log = fopen(LOG_SAVE_FILE, "a+");        \
        if( fp_log )                                \
        {                                           \
            time(&cur_tm);                          \
            fprintf(fp_log, "%.24s ["#log_type"] [PID=%d] [TID=%ld] %s():%d "format, ctime(&cur_tm), currentPID, currentTID, __FUNCTION__, __LINE__, args); \
            fclose(fp_log);                         \
        }                                           \
    }

#define LOG_SAVE_MSG(log_type, msg)                 \
    if( log_type )                                  \
    {                                               \
        fp_log = fopen(LOG_SAVE_FILE, "a+");        \
        if( fp_log )                                \
        {                                           \
            time(&cur_tm);                          \
            fprintf(fp_log, "%.24s ["#log_type"] [PID=%d] [TID=%ld] %s():%d "msg, ctime(&cur_tm), currentPID, currentTID, __FUNCTION__,  __LINE__); \
            fclose(fp_log);                         \
        }                                           \
    }


#define LOG_CHECK_DECLARE()                         FILE* fp_chk;
#define LOG_CHECK_1                                 "LOG_ENABLE_1"
#define LOG_CHECK_1_START()                         \
    fp_chk = fopen(LOG_CHECK_1, "r");               \
    if( fp_chk )                                    \
    {                                               \
        fclose(fp_chk);
#define LOG_CHECK_2                                 "LOG_ENABLE_2"
#define LOG_CHECK_2_START()                         \
    fp_chk = fopen(LOG_CHECK_2, "r");               \
    if( fp_chk )                                    \
    {                                               \
        fclose(fp_chk);
#define LOG_CHECK_END()                             \
    }

/*
LOG_DECLARE();
LOG_CHECK_DECLARE();

LOG_PRINT_FORMAT(LOG_DEBUG, "%s", msg);
LOG_PRINT_MSG(LOG_DEBUG, "");

LOG_SAVE_FORMAT(LOG_DEBUG, "%s", msg);
LOG_SAVE_MSG(LOG_DEBUG, "");



LOG_CHECK_1_START()
LOG_PRINT_FORMAT(LOG_DEBUG, "%s", msg);
LOG_PRINT_MSG(LOG_DEBUG, "");

LOG_SAVE_FORMAT(LOG_DEBUG, "%s", msg);
LOG_SAVE_MSG(LOG_DEBUG, "");
LOG_CHECK_END()



LOG_CHECK_2_START()
LOG_PRINT_FORMAT(LOG_DEBUG, "%s", msg);
LOG_PRINT_MSG(LOG_DEBUG, "");

LOG_SAVE_FORMAT(LOG_DEBUG, "%s", msg);
LOG_SAVE_MSG(LOG_DEBUG, "");
LOG_CHECK_END();
*/

#endif    //LOGGER_H
