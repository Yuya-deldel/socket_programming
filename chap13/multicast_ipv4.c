#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 12345
#define MULTICAST_ADDR "239.192.1.2"

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    int ttl = 10;   // setting TTL: 10 回ルーターを超えられる
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) != 0) {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, MULTICAST_ADDR, &addr.sin_addr.s_addr);

    sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr));

    close(sock);
    return 0;
}