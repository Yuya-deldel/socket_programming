#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define PORT_NUM 12345
#define NOT_EXIST_ADDR "192.168.0.0"

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, NOT_EXIST_ADDR, &server.sin_addr.s_addr);

    struct timeval tv;      // timeout = 1sec
    tv.tv_sec = 1;          // sec
    tv.tv_usec = 0;         // micro sec
    
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));

    // 存在しないアドレスに connect() して強制的に timeout させる
    connect(sock, (struct sockaddr *)&server, sizeof(server));
    perror("connect");

    close(sock);
    return 0;
}