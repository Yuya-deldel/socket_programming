#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#define ADDR "255.0.0.0"

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct ifreq ifr;
    struct sockaddr_in *s_in = (struct sockaddr_in *)&ifr.ifr_addr;
    s_in->sin_family = AF_INET;
    inet_pton(AF_INET, ADDR, &s_in->sin_addr);
    strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE-1);
    
    if (ioctl(fd, SIOCSIFNETMASK, &ifr) != 0) perror("ioctl");

    close(fd);
    return 0;
}