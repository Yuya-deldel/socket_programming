#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define PORT_NUM "12345"

int main() {
    // res <- address info
    struct addrinfo hints, *res;
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    int err = getaddrinfo(NULL, PORT_NUM, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    // create socket
    int sock0 = socket(res->ai_family, res->ai_socktype, 0);
    if (sock0 < 0) {
        perror("socket");
        return 1;
    }

    // socket を再利用可能に設定: プログラムを繰り返し実行 & 終了してもよいようにする
    int yes = 1;
    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    if (bind(sock0, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind");
        return 1;
    }
    freeaddrinfo(res);

    if (listen(sock0, 5) != 0) {
        perror("listen");
        return 1;
    }

    // http message
    char buf[2048], inbuf[2048];
    snprintf(buf, sizeof(buf), 
        "HTTP/1.0 200 OK\r\nContent-Length: 20\r\nContent-Type: text/html\r\n\r\nHELLO\r\n"
    );

    int sock, len;
    struct sockaddr_in client;
    while (1) {
        // accept
        len = sizeof(client);
        sock = accept(sock0, (struct sockaddr *)&client, &len);
        if (sock < 0) {
            perror("accept");
            break;
        }

        int n = read(sock, inbuf, sizeof(inbuf));       // data 受信
        write(fileno(stdout), inbuf, n);                // stdout に受信データ表示
        write(sock, buf, (int)strlen(buf));             // 用意しておいたメッセージを出力

        close(sock);
    }   // 到達しない

    close(sock0);
    return 0;
}