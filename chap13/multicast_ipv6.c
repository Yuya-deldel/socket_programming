#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 12345
#define MULTICAST_ADDR "ff12::1111:2222"

// setting TTL
int main() {
    int sock = socket(AF_INET6, SOCK_DGRAM, 0);

    int hops = 10;   // 10 回ルーターを超えられる
    if (setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char *)&hops, sizeof(hops)) != 0) {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(PORT_NUM);
    inet_pton(AF_INET6, MULTICAST_ADDR, &addr.sin6_addr);

    sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr));

    close(sock);
    return 0;
}