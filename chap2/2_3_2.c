#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void print_port(int sock) {
    char buf[48];
    struct sockaddr_in s;
    socklen_t sz = sizeof(s);
    
    if (getsockname(sock, (struct sockaddr *)&s, &sz) != 0) {
        perror("getsockname");
        return;
    }

    inet_ntop(AF_INET, &s.sin_addr, buf, sizeof(buf));
    printf("%s : %d\n", buf, ntohs(s.sin_port));
}

int main() {
    char buf[1024];
    struct sockaddr_in peer;
    socklen_t peerlen = sizeof(peer);

    int sock_0 = socket(AF_INET, SOCK_STREAM, 0);

    // bind() せずに listen() する
    if (listen(sock_0, 5) != 0) {
        perror("listen");
        return 1;
    }

    print_port(sock_0);     // 自動的に未使用ポートが割り当てられる

    int sock = accept(sock_0, (struct sockaddr *)&peer, &peerlen);
    if (sock < 0) {
        perror("accept");
        return 1;
    }

    write(sock, "HOGE\n", 5);

    close(sock);
    close(sock_0);
    return 0;
}