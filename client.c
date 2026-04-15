/*
 * 文件名称: client.c
 * 描述: AetherHome 8080 TCP客户端（优化版，完整错误处理）
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 1024

int client_sock = 0;

// 信号处理：Ctrl+C 优雅退出
void sigint_handler(int sig) {
    if (client_sock > 0) {
        close(client_sock);
        printf("\n客户端已退出\n");
    }
    exit(0);
}

int main() {
    struct sockaddr_in serv_addr;
    char send_buf[BUF_SIZE] = {0};
    char recv_buf[BUF_SIZE] = {0};

    // 注册信号处理
    signal(SIGINT, sigint_handler);

    // 1. 创建套接字
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("socket failed");
        return -1;
    }

    // 2. 配置服务器地址
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        close(client_sock);
        return -1;
    }

    // 3. 连接服务器
    if (connect(client_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        close(client_sock);
        return -1;
    }

    printf("成功连接服务器 %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("输入消息发送，输入 exit 退出\n\n");

    // 4. 通信循环
    while (1) {
        memset(send_buf, 0, BUF_SIZE);
        memset(recv_buf, 0, BUF_SIZE);

        // 读取用户输入
        printf("客户端 > ");
        if (fgets(send_buf, BUF_SIZE, stdin) == NULL) {
            perror("读取输入失败");
            break;
        }

        // 去除换行符
        send_buf[strcspn(send_buf, "\n")] = '\0';

        // 退出指令
        if (strcmp(send_buf, "exit") == 0) {
            printf("客户端主动退出\n");
            break;
        }

        // 发送数据
        ssize_t send_len = send(client_sock, send_buf, strlen(send_buf), 0);
        if (send_len < 0) {
            perror("发送数据失败");
            break;
        } else if (send_len == 0) {
            printf("服务器已断开连接\n");
            break;
        }

        // 接收响应
        ssize_t recv_len = recv(client_sock, recv_buf, BUF_SIZE - 1, 0);
        if (recv_len < 0) {
            perror("接收数据失败");
            break;
        } else if (recv_len == 0) {
            printf("服务器已断开连接\n");
            break;
        }

        // 打印服务器响应
        printf("服务器 > %s\n", recv_buf);
    }

    close(client_sock);
    return 0;
}
