#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "basedef.h"

/*
获得文件大小
@param filename [in]: 文件名
@return 文件大小
*/
long get_file_size(char* filename)
{
    long length = 0;
    FILE *fp = NULL;
    fp = fopen(filename, "rb");
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
    }
    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }
    return length;
}

/*
获得当前时间字符串
@param buffer [out]: 时间字符串
@return 空
*/
void get_local_time(char* buffer)
{
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
    (timeinfo->tm_year+1900), timeinfo->tm_mon+1, timeinfo->tm_mday,
    timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

/*
写入日志文件
@param filename [in]: 日志文件名
@param max_size [in]: 日志文件大小限制
@param buffer [in]: 日志内容
@param buf_size [in]: 日志内容大小
@return 空
*/
void write_log_file(char* buffer, unsigned buf_size, char* fname)
{
    FILE *fp;
    char now[64];
    // 文件超过最大限制, 删除
    long length = get_file_size(fname);
    if (length > FILE_MAX_SIZE) {
        unlink(fname); // 删除文件
    }
    // 写日志
    fp = fopen(fname, "at+");
    if (fp != NULL) {
        memset(now, 0, sizeof(now));
        get_local_time(now);
        strcat(now, "---->>>> ");
        fwrite(now, strlen(now)+1, 1, fp);
        fwrite(buffer, buf_size, 1, fp);
        fclose(fp);
        fp = NULL;
    }
}

void debugToStderr(const char* file, const char* func, u32 line, const char *fmt, ...)
{
    va_list ap;
    char bufTime[20] = { 0 };

    get_local_time(bufTime);
    fprintf(stderr, "\n[%s][%s][%s()][%d]: ", bufTime, file, func, line);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
}

void debugBuf(const char* file, const char* func, u32 line, u8* buf, u32 bufSize)
{
    u32 i=0;
    char bufTime[20] = { 0 };

    if(NULL == buf)
        return;

    get_local_time(bufTime);
    fprintf(stderr, "\n[%s][%s][%s()][%d]: ", bufTime, file, func, line);

    for(i=0;i<bufSize-1;i++)
        fprintf(stderr, "%02X ", *(buf+i));
    fprintf(stderr, "%02X\n", *(buf+i));
}

void debugToFile(const char* fname, const char* file, const char* func, u32 line, const char *fmt,...)
{
    va_list ap;
    char bufTime[20] = { 0 };
    FILE *fp = NULL;

    if(NULL == fname)
        return;

    fp = fopen(fname, "a+");
    if (fp != NULL) {
        get_local_time(bufTime);
        fprintf(fp, "\n[%s][%s][%s()][%d]: ", bufTime, file, func, line);
        va_start(ap, fmt);
        vfprintf(fp, fmt, ap);
        va_end(ap);
        fprintf(fp, "\n");
        fflush(fp);
        fclose(fp);
    }
}

void printBuf(u8* buf, u32 bufSize, u8 space, u8 inverse)
{
    u32 i = 0;

    if (NULL == buf || 0 == bufSize) {
        return;
    }

    if(inverse == 0)
        for (i = 0; i < (bufSize - 1); i++)
            fprintf(stderr, "%02X%s", buf[i], space==0?"":" ");
    else
        for (i = (bufSize - 1); i > 0; i--)
            fprintf(stderr, "%02X%s", buf[i], space==0?"":" ");

    fprintf(stderr, "%02X\n", buf[i]);
}
