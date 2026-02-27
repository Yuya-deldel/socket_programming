#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>

// interface は down 状態である必要がある

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    // 設定
    struct ifreq ifr;
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;    // ethernet 
    ifr.ifr_hwaddr.sa_data[0] = 0xAA;
    ifr.ifr_hwaddr.sa_data[1] = 0xBB;
    ifr.ifr_hwaddr.sa_data[2] = 0xCC;
    ifr.ifr_hwaddr.sa_data[3] = 0xDD;
    ifr.ifr_hwaddr.sa_data[4] = 0xEE;
    ifr.ifr_hwaddr.sa_data[5] = 0xFF;
    strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE-1);

    if (ioctl(fd, SIOCSIFHWADDR, &ifr) != 0) {
        perror("ioctl");
        return 1;
    }

    close(fd);
    return 0;
}