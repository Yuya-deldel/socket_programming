#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <linux/if_link.h>      // 本文中にはないが必要

#define MSG_SEQ 100

int main() {
    // create netlink socket
    int sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

    // create sockaddr_nl structure
    struct sockaddr_nl sa;
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_pid = 0;       // kernel
    sa.nl_groups = 0;    // unicast

    // create request message to kernel
    struct {
        struct nlmsghdr nh;
        struct ifinfomsg ifi;
    } request;
    memset(&request, 0, sizeof(request));
    request.nh.nlmsg_len = sizeof(request);
    request.nh.nlmsg_type = RTM_GETLINK;
    request.nh.nlmsg_flags = NLM_F_ROOT | NLM_F_REQUEST;
    request.nh.nlmsg_pid = 0;       // to kernel
    request.nh.nlmsg_seq = MSG_SEQ;
    request.ifi.ifi_family = ARPHRD_ETHER;

    // send netlink message to kernel
    int n = sendto(sock, (void *)&request, sizeof(request), 0, (struct sockaddr *)&sa, sizeof(sa));
    if (n < 0) {
        perror("sendto");
        return 1;
    }

    // receive reply message from kernel
    char buf[4096];
    n = recv(sock, buf, sizeof(buf), 0);
    if (n < 0) {
        perror("recv msg");
        return 1;
    }
    printf("received netlink message\n");

    // analyze netlink message
    struct nlmsghdr *nlhdr;
    for (nlhdr = (struct nlmsghdr *)buf; NLMSG_OK(nlhdr, n); nlhdr = NLMSG_NEXT(nlhdr, n)) {
        // print netlink info
        printf("##########\n");
        printf("<total netlink message>\n");
        printf("length: %d\n", nlhdr->nlmsg_len);

        if (nlhdr->nlmsg_type != RTM_NEWLINK) {     // RTM_GETLINK への正常な reply
            printf("type error: %d\n", nlhdr->nlmsg_type);
            continue;
        } 
        printf("type  : %d\n", nlhdr->nlmsg_type);

        // netlink message のデータ部分を抽出
        struct ifinfomsg *ifimsg = NLMSG_DATA(nlhdr);
        if (ifimsg->ifi_family != AF_UNSPEC && ifimsg->ifi_family != AF_INET6) {
            printf("error family: %d\n", ifimsg->ifi_family);
            continue;
        }

        // print parameters
        printf("<ifinfomsg data>\n");
        printf("    family: %d\n", ifimsg->ifi_family);
        printf("    type  : %d\n", ifimsg->ifi_type);
        printf("    index : %d\n", ifimsg->ifi_index);
        printf("    flags : %d\n", ifimsg->ifi_flags);
        printf("    change: %d\n", ifimsg->ifi_change);

        // analyze rtattr message
        int rtalist_len = nlhdr->nlmsg_len - NLMSG_LENGTH(sizeof(struct ifinfomsg));
        struct rtattr *rta;
        for (rta = IFLA_RTA(ifimsg); RTA_OK(rta, rtalist_len); rta = RTA_NEXT(rta, rtalist_len)) {
            printf("rtattr type: %d\n", rta->rta_type);
            
            switch (rta->rta_type) {
            case IFLA_IFNAME:
                printf("    + %s\n", (char *)RTA_DATA(rta));
                break;

            case IFLA_MTU:
                printf("    + MTU: %d\n", *(int *)RTA_DATA(rta));
                break;

            case IFLA_LINK:
                printf("    + Link type: %d\n", *(int *)RTA_DATA(rta));
                break;

            case IFLA_ADDRESS:
            case IFLA_BROADCAST:
                {
                    unsigned char *a = RTA_DATA(rta);
                    if (RTA_PAYLOAD(rta) == 6) {
                        printf("    + %s: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
                            rta->rta_type == IFLA_ADDRESS ? "Address" : "Broadcast", a[0], a[1], a[2], a[3], a[4], a[5]
                        );
                    }
                }
                break;
            
            case IFLA_STATS:
                {
                    struct rtnl_link_stats *nds = RTA_DATA(rta);
                    if (RTA_PAYLOAD(rta) != (int)sizeof(struct rtnl_link_stats)) {
                        printf("error in IFLA_STATS");
                        break;
                    }
                    
                    printf("    + stats: rxpkt=%u, txpkt=%u\n", nds->rx_packets, nds->tx_packets);
                }    
                break;
            default:
                printf("    + other type: %d\n", rta->rta_type);
                break;
            }
        }
    } 

    close(sock);
    return 0;
}