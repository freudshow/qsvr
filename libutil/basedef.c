#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "basedef.h"

static const u16 wCRCTable[] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

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
    if (length > LOG_COUNT) {
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

/**************************************************
 * 功能描述: 获得当前日志目录下, 日志后缀数值最大的号码. 例如, 日志目录
 * 为 /var/logs/, 日志文件名的前缀是"1376log.", 假如当前时刻目录
 * "/var/logs/"下有文件"1376log.1", "1376log.2",
 *  "1376log.3", 3个文件, 那么此函数返回整数3
 * ------------------------------------------------
 * 输入参数: fname [in]: 日志文件存放的目录名
 * 输出参数: 无
 * ------------------------------------------------
 * 返回值: 成功, 返回日志文件后缀数的最大值; 失败, 返回 -1
 * ------------------------------------------------
 * 修改日志:
 * 		1. 日期: 2020年7月19日
 创建函数
 **************************************************/
int getMaxFileNo(const char *fname)
{
    int ret = 0;
    char cmd[100] = { 0 }; //调用shell命令
    char result[128] = { 0 };
    FILE *fstream = NULL;

    sprintf(cmd, "ls -r %s*|cut -c %d-99", fname, (int) strlen(fname) + 2); //最大两位数

    if (NULL == (fstream = popen(cmd, "r"))) { //打开管道
        fprintf(stdout, "execute command failed: %s", strerror(errno));
        return -1;
    }

    if (0 != fread(result, sizeof(char), sizeof(result), fstream)) { //读取执行结果
        int i = 0;
        while (result[i] != '\n') //读取第一个数字, 'ls -r' 命令默认降序排列
            i++;

        result[i] = 0; //字符串结束符
        ret = atoi(&result[0]);
    } else {
        ret = -1;
    }

    pclose(fstream);

    return ret;
}

/**************************************************
 * 功能描述: 检查当前时刻, 日志目录下的日志文件是否达到设定的最大文件数,
 * 如果已经达到了最大文件数, 则删除后缀值最大的文件, 且将其前序文件的后
 * 缀值依次增1. 例如, 当前设定的最大日志文件数为3, 且当前日志文件目录
 * 下的日志文件数已达到3个: log.0, log.1, log.2, 那么就将文件
 * "log.2"删掉, 依次将log.0, log.1的文件名修改为log.1,
 * log.2, 以保持日志记录的时间顺序
 * ------------------------------------------------
 * 输入参数: fname [in]: 日志文件名的前缀
 * 输入参数: logCount [in]: 设定的日志文件的最大数
 * 输出参数: 无
 * ------------------------------------------------
 * 返回值: 成功, 返回0; 失败, 返回 -1
 * ------------------------------------------------
 * 修改日志:
 * 		1. 日期: 2020年7月19日
 创建函数
 **************************************************/
int mvFiles(const char *fname, int logCount)
{
    if ((access(fname, F_OK)) != 0) {
        return -1;
    }


    int max = getMaxFileNo(fname);
    int i = 0;

    if (max < 0)
        return -1;

    if ((max + 1) == logCount) {
        char name[100] = { 0 };
        sprintf(name, "%s.%d", fname, max);
        max -= 1;
        unlink(name);
//        sync();
    }

    char cmd[200] = { 0 };
    for (i = max; i > 0; i--) {
        sprintf(cmd, "mv %s.%d %s.%d", fname, i, fname, i + 1);

        if (system(cmd) == -1) {
            printf("mvFiles err \r\n");
            return -1;
        }
    }

    if (logCount > 1) {
        sprintf(cmd, "mv %s %s.%d", fname, fname, i + 1);

        if (system(cmd) == -1) {
            printf("mvFiles err \r\n");
            return -1;
        }
    }

    return 0;
}

/**************************************************
 * 功能描述: 检查当前时刻, 日志目录下的日志文件是否达到设定的最大文件数,
 * 如果已经达到了最大文件数, 则删除后缀值最大的文件, 且将其前序文件的后
 * 缀值依次增1. 例如, 当前设定的最大日志文件数为3, 且当前日志文件目录
 * 下的日志文件数已达到3个: log.0, log.1, log.2, 那么就将文件
 * "log.2"删掉, 依次将log.0, log.1的文件名修改为log.1,
 * log.2, 以保持日志记录的时间顺序
 * ------------------------------------------------
 * 输入参数: fname [in]: 文件名, 绝对路径
 * 输入参数: logsize [in]: 日志文件的最大长度
 * 输入参数: logCount [in]: 日志文件的最大个数
 * 输出参数: 无
 * ------------------------------------------------
 * 返回值: 成功, 返回0; 失败, 返回 -1
 * ------------------------------------------------
 * 修改日志:
 * 		1. 日期: 2020年7月19日
 创建函数
 **************************************************/
int logLimit(const char *fname, int logsize, int logCount)
{
    struct stat fileInfo;
    int ret = 0;

    if (stat(fname, &fileInfo) == -1) {
        char cmd[128] = { 0 };
        sprintf(cmd, "rm -f %s", fname);

        if (system(cmd) == -1) {
            printf("mvFiles err \r\n");
            return -1;
        }
        return -1;
    }

    /**
     * 检查当前日志文件名的最大值(以logCount为参照)
     * 比如, logCount = 10, 如果当前文件名有'.9'
     * 结尾的, 那么就将'fname.9'删掉, 依次将'fname.8'复制
     * 为'fname9', 'fname.7'复制为'fname.8'...'fname'
     * 复制为'fname.1'; 如果当前文件没有达到最大值,
     * 则依次复制当前最大值的文件名, 往后累加一个.
     * 所以, 最关键的是获取当前最大的文件名.
     */
    if (fileInfo.st_size > logsize) {
        ret = mvFiles(fname, logCount);
    }

    return ret;
}

/**************************************************
 * 功能描述: 将字节数组, 以16进制格式, 打印到字符串out中, 以空格
 * 分开每个字节
 * ------------------------------------------------
 * 输入参数: buf [in]: 字节数组
 * 输入参数: len [in]: 字节数组长度
 * 输出参数: out, 输出的字符串
 * ------------------------------------------------
 * 返回值: 无
 * ------------------------------------------------
 * 修改日志:
 * 		1. 日期: 2020年7月19日
 创建函数
 **************************************************/
void getBufString(char *buf, int len, char *out)
{
    int i = 0;
    char s[5] = { 0 };
    for (i = 0; i < len; i++) {
        sprintf(s, "%02X ", (unsigned char) buf[i]);
        strcat(out, s);
    }
}

/**************************************************
 * 功能描述: 将调试信息打印到文件描述符fp中
 * ------------------------------------------------
 * 输入参数: fp [in]: 文件描述符
 * 输入参数: file [in]: 调试信息所在的文件名
 * 输入参数: func [in]: 调试信息所在的函数名
 * 输入参数: line [in]: 调试信息所在的行号
 * 输出参数: 无
 * ------------------------------------------------
 * 返回值: 无
 * ------------------------------------------------
 * 修改日志:
 * 		1. 日期: 2020年7月19日
 创建函数
 **************************************************/
void debugBufFormat2fp(FILE *fp, const char *file, const char *func, int line,
                       char *buf, int len, const char *fmt, ...)
{
    va_list ap;
    char bufTime[20] = { 0 };

    if (fp != NULL) {
        get_local_time(bufTime);
        fprintf(fp, "\n[%s][%s][%s()][%d]: ", bufTime, file, func, line);
        va_start(ap, fmt);
        vfprintf(fp, fmt, ap);
        va_end(ap);

        if (buf != NULL && len > 0) {
            char *s = (char*) calloc(1, 3 * (len + 1)); //多开3个字符的余量
            if (s != NULL) {
                getBufString(buf, len, s);
                fprintf(fp, "%s\n", s);
            }
            free(s);
        }
        fprintf(fp, "\n");
        fflush(fp);
        if (fp != stdout && fp != stderr)
            fclose(fp);
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


u16 calcModRtuCRC(u8 *buf, int len)
{
    u16 crc = 0xFFFF;
    int pos = 0;
    int i = 0;

    for (pos = 0; pos < len; pos++) {
        crc ^= (u16)buf[pos];

        for (i = 8; i > 0; i--) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= POLY;
            }
            else
                crc >>= 1;
        }
    }

    return crc;
}

u16 crc16continue(u8 *buf, int len, u16 crc, u8 lastCrc)
{
    u8 nTemp;
    u16 wCRCWord = crc;

    if (lastCrc == 0)
    {
        wCRCWord = 0xFFFF;
    }

    while (len--)
    {
        nTemp = (*buf++ ^ (u8)(wCRCWord));
        wCRCWord >>= 8;
        wCRCWord  ^= wCRCTable[nTemp];
    }

    return wCRCWord;
}

u16 crc16TblDrv(const u8 *nData, int wLength)
{
    u8 nTemp;
    u16 wCRCWord = 0xFFFF;

    while (wLength--){
        nTemp = (*nData++ ^ (u8)(wCRCWord));
        wCRCWord >>= 8;
        wCRCWord  ^= wCRCTable[nTemp];
    }

    return wCRCWord;
}

u16 pppfcs16(u16 fcs,unsigned char *cp, int len)
{
    static u16 fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
    };

    while (len--)
    fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
    return (fcs);
}

/*tryfcs16   bg
* How to use the fcs
*/
#define PPPINITFCS16 0xffff /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8 /* Good final FCS value */

u16 fcs16(unsigned char *cp, int  len)
{
    u16 fcs=0;
    /* add on output */
    fcs = pppfcs16( PPPINITFCS16, cp, len );
    fcs ^= 0xffff; /* complement */

    return fcs;
}

u8 chkSum(u8* buf, u16 bufSize)
{
    u16 i = 0;
    u8 sum = 0;

    for (i = 0, sum = 0; i < bufSize; i++)
        sum += buf[i];

    return sum;
}

u8 xorSum(u8* buf, int bufSize)
{
    u16 i = 0;
    u8 sum = 0;

    for (i = 0, sum = 0; i < bufSize; i++)
        sum ^= buf[i];

    return sum;
}

void add33(u8* buf, int bufSize)
{
    u16 i = 0;

    for (i = 0; i < bufSize; i++)
        buf[i] += 0x33;
}

void minus33(u8* buf, int bufSize)
{
    u16 i = 0;

    for (i = 0; i < bufSize; i++)
        buf[i] -= 0x33;
}

void inverseArray(u8* buf, u16 bufSize)
{
    u16 i = 0;
    for (i = 0; i < bufSize / 2; i++) {//swap two symmetric elements
        SWAP(buf[i], buf[bufSize - i - 1]);
    }
}

