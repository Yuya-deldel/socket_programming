#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT_NUM 12345

void print_port(int sock) {
    struct sockaddr_in s;
    socklen_t sz = sizeof(s);
    getsockname(sock, (struct sockaddr *)&s, &sz);
    printf("%d\n", ntohs(s.sin_port));
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s dest_IP_address\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);

    if (sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr)) < 1) {
        perror("sendto");
        return 1;
    }

    print_port(sock);

    char buf[2048];
    memset(buf, 0, sizeof(buf));
    struct sockaddr_in senderinfo;
    socklen_t senderinfolen = sizeof(senderinfo);
    recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)&senderinfo, &senderinfolen);

    printf("%s\n", buf);
    
    close(sock);
    return 0;
}