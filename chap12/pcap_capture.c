// compile 時 pcap package に link する; gcc pcap_*.c -lpcap

#include <pcap.h>
#include <string.h>
#include <time.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>

void pcap_handler_func(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) {
    if (h->caplen < sizeof(struct iphdr) + sizeof(struct ether_header)) return;

    char timebuf[64];
    memset(timebuf, 0, sizeof(timebuf));
    if (ctime_r(&h->ts.tv_sec, timebuf) == NULL) return;
    timebuf[strlen(timebuf) - 1] = '\0';    // \n を削除

    printf("%s, caplen:%d, len:%d, ", timebuf, h->caplen, h->len);

    struct ether_header *ethhdr = (struct ether_header *)bytes;
    char addrstr[64];
    switch (ntohs(ethhdr->ether_type)) {
        case ETHERTYPE_IP:
        printf("IPv4 ");
        struct iphdr *ipv4h = (struct iphdr *)(bytes + sizeof(struct ether_header));
        inet_ntop(AF_INET, &ipv4h->saddr, addrstr, sizeof(addrstr));
        printf("src[%s]\n", addrstr);
        break;

        case ETHERTYPE_IPV6:
        printf("IPv6 ");
        struct ip6_hdr *ipv6h = (struct ip6_hdr *)(bytes + sizeof(struct ether_header));;
        inet_ntop(AF_INET6, (void *)&ipv6h->ip6_src, addrstr, sizeof(addrstr));
        printf("src[%s]\n", addrstr);
        break;

        default:
        printf("other\n");
        break;
    }
    return;
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *p = pcap_open_live("lo", 65536, 1, 10, errbuf);
    if (p == NULL) {
        fprintf(stderr, "%s\n", errbuf);
        return 1;
    }

    if (pcap_loop(p, -1, pcap_handler_func, NULL) < 0) {        // packet が到着するたびに呼ばれる
        fprintf(stderr, "%s\n", pcap_geterr(p));
        pcap_close(p);
        return 1;
    }

    pcap_close(p);
    return 0;
}