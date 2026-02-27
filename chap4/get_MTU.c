#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

// Maximum Transmission Unit: 一度のデータ送信で送信可能な最大サイズ

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct ifreq ifr;
    strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE-1);

    if (ioctl(fd, SIOCGIFMTU, &ifr) != 0) {
        perror("ioctl");
        return 1;
    }

    close(fd);

    printf("%d\n", ifr.ifr_mtu);

    return 0;
}