#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#define DELETE_ADDR "2003::1"

struct in6_ifreq {
    struct in6_addr ifr6_addr;
    u_int32_t ifr6_prefixlen;
    int ifr6_ifindex;
};

int main() {
    int fd = socket(AF_INET6, SOCK_DGRAM, 0);

    struct in6_ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    inet_pton(AF_INET6, DELETE_ADDR, &ifr.ifr6_addr);
    ifr.ifr6_prefixlen = 64;
    ifr.ifr6_ifindex = if_nametoindex("eth0");
    
    if (ioctl(fd, SIOCDIFADDR, &ifr) < 0) {     // 変更点
        perror("ioctl");
        return 1;
    }

    close(fd);
    return 0;
}