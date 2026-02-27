#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct ifreq ifr;
    strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE-1);

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != 0) {
        perror("ioctl");
        return 1;
    }

    close(fd);

    printf("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
        (unsigned char)ifr.ifr_hwaddr.sa_data[0],
        (unsigned char)ifr.ifr_hwaddr.sa_data[1],
        (unsigned char)ifr.ifr_hwaddr.sa_data[2],
        (unsigned char)ifr.ifr_hwaddr.sa_data[3],
        (unsigned char)ifr.ifr_hwaddr.sa_data[4],
        (unsigned char)ifr.ifr_hwaddr.sa_data[5]
    );

    return 0;
}