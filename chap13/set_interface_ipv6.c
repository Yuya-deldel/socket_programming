#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 12345
#define MULTICAST_ADDR "ff12::1111:2222"

int main() {
    int sock = socket(AF_INET6, SOCK_DGRAM, 0);

    struct sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(PORT_NUM);
    inet_pton(AF_INET6, MULTICAST_ADDR, &addr.sin6_addr);

    int ifidx = if_nametoindex("eth0");     // interface 番号を指定
    if (setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char *)&ifidx, sizeof(int)) != 0) {
        perror("setsockopt");
        return 1;
    }

    if (sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr)) < 1) {
        perror("sendto");
        return 1;
    }

    close(sock);
    return 0;
}