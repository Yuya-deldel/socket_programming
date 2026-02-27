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

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) != 0) {   // ifr <- eth0 の現在の状態
        perror("ioctl");
        return 1;
    }

    // 状態を down にする (interface を無効化する)
    ifr.ifr_flags &= ~IFF_UP;
    if (ioctl(fd, SIOCSIFFLAGS, &ifr) != 0) {
        perror("ioctl");
        return 1;
    }

    close(fd);
    return 0;
}