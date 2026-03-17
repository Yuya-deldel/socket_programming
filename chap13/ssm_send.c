#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT_NUM "12345"
#define SSM_ADDR "232.1.2.3"    // 232.0.0.0/8: SSM

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s my_ipv4_addr\n", argv[0]);
        return 1;
    }

    // 送信元の情報を探す
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    int err = getaddrinfo(argv[1], NULL, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 送信元の情報も含めて bind する
    if (bind(sock, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind");
        return 1;
    }
    freeaddrinfo(res);

    printf("SSM SOURCE: %s\n", argv[1]);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    err = getaddrinfo(SSM_ADDR, PORT_NUM, &hints, &res);  
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    // SSM address に対し送信
    if (sendto(sock, "HELLO", 5, 0, res->ai_addr, res->ai_addlren) < 0) {
        perror("sendto");
        return 1;
    }

    close(sock);
    freeaddrinfo(res);
    return 0;
}