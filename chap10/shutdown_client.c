// shutdown: 自分側の socket の書き込み機能を閉じる -> 安全な通信終了を実現

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define PORT_NUM 12345
#define LOCAL_HOST "127.0.0.1"

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, LOCAL_HOST, &server.sin_addr.s_addr);

    connect(sock, (struct sockaddr *)&server, sizeof(server));

    write(sock, "HOGE\n", 6);

    shutdown(sock, SHUT_WR);    // 書き込み終了状態

    char buf[32];
    if (read(sock, buf, sizeof(buf))) printf("closed by peer\n");

    close(sock);
    return 0;
}