#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT_NUM 12345

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, "239.192.1.2", &addr.sin_addr.s_addr);   // multi-cast address

    if (sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr)) < 1) {
        perror("sendto");
        return 1;
    }

    close(sock);
    return 0;
}