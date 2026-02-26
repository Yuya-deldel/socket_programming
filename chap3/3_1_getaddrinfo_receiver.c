#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT_NUM "12345"

int main() {
    // res <- address data
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    int err = getaddrinfo(NULL, PORT_NUM, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    // create IPv4 UDP socket
    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // bind
    if (bind(sock, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind");
        return 1;
    }

    freeaddrinfo(res);

    // recv()
    char buf[2048];
    recv(sock, buf, sizeof(buf), 0);

    // program
    printf("%s\n", buf);

    close(sock);
    return 0;
}