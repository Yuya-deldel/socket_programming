#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

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
    if (argc != 4) {
        printf("usage: %s IP_ADDR\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // 宛先設定
    struct sockaddr_ll addr;
    addr.sll_family = AF_PACKET;
    addr.sll_protocol = htons(ETH_P_IP);
    addr.sll_ifindex = if_nametoindex(argv[3]);
    addr.sll_halen = IFHWADDRLEN;
    memset(&addr.sll_addr, 0xff, IFHWADDRLEN);      // broadcast address: FF:FF:FF:FF:FF:FF; 本来は不適切
    
    char buf[2000];
    memset(&buf, 0, sizeof(buf));
    struct iphdr *iphdrptr = (struct iphdr *)buf; 
    iphdrptr->ttl = 32;
    iphdrptr->protocol = IPPROTO_ICMP;
    iphdrptr->ihl = sizeof(struct iphdr) / 4;
    iphdrptr->version = 4;
    iphdrptr->tot_len = htons(sizeof(struct iphdr));
    iphdrptr->id = random();
    inet_pton(AF_INET, argv[1], &iphdrptr->saddr);     // source
    inet_pton(AF_INET, argv[2], &iphdrptr->daddr);      // destination
    iphdrptr->check = checksum((unsigned short *)iphdrptr, sizeof(iphdrptr));

    struct icmphdr *icmphdrptr = (struct icmphdr *)(buf + sizeof(struct iphdr));
    icmphdrptr->type = ICMP_ECHO;
    icmphdrptr->code = 0;
    icmphdrptr->un.echo.id = 0;
    icmphdrptr->un.echo.sequence = 0;
    icmphdrptr->checksum = 0;
    icmphdrptr->checksum = checksum((unsigned short *)icmphdrptr, sizeof(struct icmphdr));

    // send ICMP packet (without data)
    if (sendto(sock, (char *)&iphdrptr, sizeof(struct iphdr) + sizeof(struct icmphdr), 
        0, (struct sockaddr *)&addr, sizeof(addr)) < 1) {
        perror("sendto");
        return 1;
    }

    // receive ICMP packet
    memset(buf, 0, sizeof(buf));
    if (recv(sock, buf, sizeof(buf), 0) < 1) {
        perror("recv");
        return 1;
    }

    iphdrptr = (struct iphdr *)buf;
    /* check whether received data is ICMP packet or not */

    // get pointer to ICMP header 
    icmphdrptr = (struct icmphdr *)(buf + (iphdrptr->ihl * 4));

    if (icmphdrptr->type == ICMP_ECHOREPLY) {
        printf("received ICMP ECHO REPLY\n");
    } else {
        printf("received ICMP %d\n", icmphdrptr->type);
    }

    close(sock);
    return 0;
}