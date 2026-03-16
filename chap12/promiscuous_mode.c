#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>

int main() {
    int sock = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // membership request 構造体
    struct packet_mreq mreq;
    memset(&mreq, 0, sizeof(mreq));
    mreq.mr_type = PACKET_MR_PROMISC;
    mreq.mr_ifindex = if_nametoindex("eth0");

    // promiscuous mode に設定
    if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) != 0) {
        perror("setsockopt");
        return 1;
    }

    char buf[2048];
    if (recv(sock, buf, sizeof(buf), 0) < 1) {
        perror("recv");
        return 0;
    }

    struct iphdr *ip = (struct iphdr *)buf;

    printf("IP version: %d\n", ip->version);
    printf("protocol: %d\n", ip->protocol);
    printf("source addr: %.8x\n", ip->saddr);
    printf("dest addr: %.8x\n", ip->daddr);

    return 0;
}