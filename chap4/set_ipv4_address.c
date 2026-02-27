#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>

#define NEW_ADDR "10.1.2.30"

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct ifreq ifr;
    struct sockaddr_in *s_new = (struct sockaddr_in *)&ifr.ifr_addr;
    s_new->sin_family = AF_INET;
    s_new->sin_addr.s_addr = inet_addr(NEW_ADDR);
    strncpy(ifr.ifr_name, "eth0", IF_NAMESIZE-1);
    
    if (ioctl(fd, SIOCSIFADDR, &ifr) != 0) perror("ioctl");

    close(fd);
    return 0;
}