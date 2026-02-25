#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr.s_addr);       // 127.0.0.1: localhost

    connect(sock, (struct sockaddr *)&server, sizeof(server));      // server に接続
    
    // data 受信
    char buf[32];
    memset(buf, 0, sizeof(buf));
    int n = read(sock, buf, sizeof(buf));
    printf("%d, %s\n", n, buf);

    close(sock);
    return 0;
}