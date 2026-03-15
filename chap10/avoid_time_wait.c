// app は終了しているはずなのに Address already in use となる場合の対処法

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 12345

int main() {
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;

    // TIME_WAIT 状態を回避して socket を再利用可能にする: SO_REUSEADDR
    int yes = 1;
    setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
    
    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock0, 5);

    struct sockaddr_in client;
    while (1) {
        socklen_t len = sizeof(client);
        int sock = accept(sock0, (struct sockaddr *)&client, &len);

        write(sock, "HELLO", 5);

        close(sock);
    }

    close(sock0);
    return 0;
}