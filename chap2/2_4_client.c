#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// 名前解決: www.example.com -> IP address (all type)
// getaddrinfo() で情報を取得し connect() を試みる
// プロトコルによらずに通信を行える
int main() {
    char *hostname = "localhost";
    char *service = "http";

    struct addrinfo hints;
    memset(&hints, 0, sizeof(sizeof(hints)));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_UNSPEC;        // all type (IPv4, IPv6, ...)

    struct addrinfo *result_0;
    int err;
    if ((err = getaddrinfo(hostname, service, &hints, &result_0)) != 0) {
        printf("error %d\n", err);
        return 1;
    }

    struct addrinfo *res;
    for (res=result_0; res!=NULL; res=res->ai_next) {
        int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) continue;

        if (connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
            close(sock);
            continue;
        }

        break;
    }
    freeaddrinfo(result_0);
    
    if (res == NULL) {
        printf("failed\n");
        return 1;
    }

    /* ... 通信を行うプログラム ... */
    
    return 0;
}