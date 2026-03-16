#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

unsigned short checksum(unsigned short *buf, int bufsize) {
    unsigned long sum = 0;
    while (bufsize > 1) {
        sum += *buf;
        buf++;
        bufsize -= 2;
    }
    if (bufsize == 1) {
        sum += *(unsigned char *)buf;
    }

    sum = (sum & 0xffff) + (sum >> 16);
    sum = (sum & 0xffff) + (sum >> 16);
    return ~sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s IP_ADDR\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);
    
    // set ICMP protocol header
    struct icmphdr hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.type = ICMP_ECHO;
    hdr.code = 0;
    hdr.un.echo.id = 0;
    hdr.un.echo.sequence = 0;
    hdr.checksum = 0;
    hdr.checksum = checksum((unsigned short *)&hdr, sizeof(hdr));

    // send ICMP packet (without data)
    if (sendto(sock, (char *)&hdr, sizeof(hdr), 0, (struct sockaddr *)&addr, sizeof(addr)) < 1) {
        perror("sendto");
        return 1;
    }

    // receive ICMP packet
    char buf[2000];
    memset(buf, 0, sizeof(buf));
    if (recv(sock, buf, sizeof(buf), 0) < 1) {
        perror("recv");
        return 1;
    }

    struct iphdr *iphdrptr = (struct iphdr *)buf;
    /* check whether received data is ICMP packet or not */

    // get pointer to ICMP header 
    struct icmphdr *icmphdrptr = (struct icmphdr *)(buf + (iphdrptr->ihl * 4));

    if (icmphdrptr->type == ICMP_ECHOREPLY) {
        printf("received ICMP ECHO REPLY\n");
    } else {
        printf("received ICMP %d\n", icmphdrptr->type);
    }

    close(sock);
    return 0;
}