/*===============================================
*   文件名称：open.c
*   创 建 者：     
*   创建日期：2026年04月14日
*   描    述：
================================================*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    int fd;
    fd = open("1.txt",O_RDWR | O_CREAT,0664);
    if(-1 == fd){
    perror("open fail\n");
    return -1;
    }
    printf("fd is %d\n",fd);
    size_t ret;
    char buf[1024] = {0};
    ret = read(fd,buf,5);
    if(ret == -1){
    perror("read fail\n");
    return -1;
    }
    printf("%s\n",buf);
    return 0;
} 
