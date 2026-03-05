#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_storage addr;
    socklen_t slen = sizeof(addr);
    printf("%d\n", slen);
    getsockname(sock, (struct sockaddr *)&addr, &slen);
    printf("%d\n", slen);

    return 0;
}