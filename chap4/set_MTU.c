#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

// Maximum Transmission Unit: 一度のデータ送信で送信可能な最大サイズ
#define NEW_MTU 1400

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct ifreq ifr;
    strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE-1);

    // 設定
    ifr.ifr_mtu = NEW_MTU;
    if (ioctl(fd, SIOCSIFMTU, &ifr) != 0) {
        perror("ioctl");
        return 1;
    }

    close(fd);
    return 0;
}