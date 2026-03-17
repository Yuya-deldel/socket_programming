#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 12345
#define MULTICAST_ADDR "239.192.1.2"
#define LOCAL_HOST "127.0.0.1"

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, MULTICAST_ADDR, &addr.sin_addr.s_addr);

    struct in_addr ipaddr;
    inet_pton(AF_INET, LOCAL_HOST, &ipaddr.s_addr);
    
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&ipaddr, sizeof(ipaddr)) != 0) {
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