#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() { 
    struct ifaddrs *ifa_list, *ifa;
    char addrstr[256], netmaskstr[256];

    if (getifaddrs(&ifa_list) != 0) {
        return 1;
    }
    for (ifa = ifa_list; ifa != NULL; ifa = ifa->ifa_next) {
        printf("name: %s\n", ifa->ifa_name);
        printf("flags: 0x%.8x\n", ifa->ifa_flags);
        
        memset(addrstr, 0, sizeof(addrstr));
        memset(netmaskstr, 0, sizeof(netmaskstr));
        if (ifa->ifa_addr->sa_family == AF_INET) {
            inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, addrstr, sizeof(addrstr));
            inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_netmask)->sin_addr, netmaskstr, sizeof(netmaskstr));
            
            printf("IPv4: %s, netmask %s\n", addrstr, netmaskstr);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr, addrstr, sizeof(addrstr));
            inet_ntop(AF_INET6, &((struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr, netmaskstr, sizeof(netmaskstr));
            
            printf("IPv6: %s, netmask %s\n", addrstr, netmaskstr);
        } else if (ifa->ifa_addr->sa_family == AF_PACKET) {
            printf("af_packet: %d\n", ifa->ifa_addr->sa_family);
        } else {
            printf("else: %d\n", ifa->ifa_addr->sa_family);
        }
        printf("\n");
    }

    freeifaddrs(ifa_list);
    return 0;
}