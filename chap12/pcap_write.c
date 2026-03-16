// compile 時 pcap package に link する; gcc pcap_*.c -lpcap

#include <pcap.h>
#include <string.h>
#include <time.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *p = pcap_open_live("lo", 65536, 1, 10, errbuf);
    if (p == NULL) {
        fprintf(stderr, "%s\n", errbuf);
        return 1;
    }

    // capture した packet 書き出し
    pcap_dumper_t *pdump;
    if ((pdump = pcap_dump_open(p, "./dump.dat")) == NULL) {
        fprintf(stderr, "%s\n", pcap_geterr(p));
        pcap_close(p);
        return 1;
    }

    // library にある pcap_dump() を呼ぶ
    if (pcap_loop(p, 10, pcap_dump, (u_char *)pdump) < 0) { 
        fprintf(stderr, "%s\n", pcap_geterr(p));
        pcap_dump_close(pdump);
        pcap_close(p);
        return 1;
    }

    pcap_dump_close(pdump);
    pcap_close(p);
    return 0;
}