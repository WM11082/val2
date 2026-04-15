/*
 * 文件名称: server.c
 * 描述: AetherHome 8080 TCP服务端（修复版，支持长连接、并发、完整错误处理 + 中文不乱码）
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <locale.h>   // 中文支持必须加

#define PORT 8080
#define BUFFER_SIZE 1024

// 客户端线程参数结构体
typedef struct {
    int sockfd;
    struct sockaddr_in addr;
} ClientInfo;

int server_fd;

// 信号处理：Ctrl+C 优雅关闭服务端
void sigint_handler(int sig) {
    if (server_fd > 0) close(server_fd);
    printf("\n服务端已关闭\n");
    exit(0);
}

// 客户端处理线程函数
void *client_thread(void *arg) {
    ClientInfo *info = (ClientInfo *)arg;
    int sockfd = info->sockfd;
    struct sockaddr_in addr = info->addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *response = "你好，来自 AetherHome 服务器！";  // 中文正常

    printf("新客户端连接！IP: %s, 端口: %d\n",
           inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    // 长连接循环：持续收发数据，直到客户端断开
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t valread = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

        // 客户端断开/读取错误
        if (valread <= 0) {
            printf("客户端 %s:%d 已断开连接\n",
                   inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            break;
        }

        // 打印收到的消息（支持中文）
        printf("收到客户端消息：%s\n", buffer);

        // 发送中文响应
        send(sockfd, response, strlen(response), 0);
        printf("已向客户端发送中文响应\n");
    }

    close(sockfd);
    free(info);
    pthread_exit(NULL);
}

int main() {
    // 【关键】启用系统中文环境
    setlocale(LC_ALL, "zh_CN.UTF-8");

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 注册信号处理
    signal(SIGINT, sigint_handler);

    // 1. 创建套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket 创建失败");
        return -1;
    }
    printf("套接字创建成功！\n");

    // 2. 配置地址信息
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 3. 绑定端口
    int ret = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if (ret < 0) {
        perror("绑定端口失败");
        close(server_fd);
        return -1;
    }
    printf("绑定端口 %d 成功！\n", PORT);

    // 4. 监听端口
    ret = listen(server_fd, 10);
    if (ret < 0) {
        perror("监听失败");
        close(server_fd);
        return -1;
    }
    printf("服务端开始监听客户端连接……\n");

    // 5. 循环接受客户端连接
    while(1) {
        printf("等待客户端连接中……\n");

        ClientInfo *info = (ClientInfo *)malloc(sizeof(ClientInfo));
        if (!info) {
            perror("内存分配失败");
            continue;
        }

        info->sockfd = accept(server_fd, (struct sockaddr *)&info->addr, (socklen_t*)&addrlen);
        if (info->sockfd < 0) {
            perror("接受客户端连接失败");
            free(info);
            continue;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_thread, (void *)info) != 0) {
            perror("线程创建失败");
            close(info->sockfd);
            free(info);
            continue;
        }

        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
