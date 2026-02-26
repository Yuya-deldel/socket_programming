#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT_NUM "12345"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s dest\n", argv[0]);
        return 1;
    }

    // res0 <- address data
    struct addrinfo hints, *res0, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // IPv4 / IPv6 対応
    hints.ai_socktype = SOCK_DGRAM;
    int err = getaddrinfo(argv[1], PORT_NUM, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    // getaddrinfo() の結果を順番に試す
    for (res=res0; res!=NULL; res=res->ai_next) {
        int sock = socket(res->ai_family, res->ai_socktype, 0);
        if (sock < 0) {
            perror("socket");
            return 1;
        }

        int n = sendto(sock, "HELLO", 5, 0, res->ai_addr, res->ai_addrlen);
        close(sock);
        if (n < 1) {
            perror("sendto");
        } else if (n > 0) {
            break;
        }
    }

    freeaddrinfo(res0);
    return 0;
}