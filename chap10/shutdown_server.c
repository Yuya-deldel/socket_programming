// shutdown: 自分側の socket の書き込み機能を閉じる -> 安全な通信終了を実現

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>

#define PORT_NUM 12345

int main() {
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock0, 5);

    struct sockaddr_in client;
    char buf[1024];
    int n;
    for (;;) {
        socklen_t len = sizeof(client);
        int sock = accept(sock0, (struct sockaddr *)&client, &len);

        while ((n = read(sock, buf, sizeof(buf))) > 0) {
            write(fileno(stdout), buf, n);
        }

        close(sock);
    }

    close(sock0);
    return 0;
}