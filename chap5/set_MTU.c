#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#define MSG_SEQ 100
#define NEW_MTU 1700

int main() {
    // create netlink socket
    int sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

    // create sockaddr_nl structure
    struct sockaddr_nl sa;
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_pid = 0;       // kernel
    sa.nl_groups = 0;    // unicast

    // create netlink handler
    char buf[4096];
    memset(buf, 0, sizeof(buf));
    struct nlmsghdr *nlhdr = (struct nlmsghdr *)buf;
    nlhdr->nlmsg_type = RTM_SETLINK;
    nlhdr->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    nlhdr->nlmsg_pid = 0;
    nlhdr->nlmsg_seq = MSG_SEQ + 1;

    struct ifinfomsg *ifihdr = NLMSG_DATA(nlhdr);
    ifihdr->ifi_family = ARPHRD_ETHER;
    ifihdr->ifi_type = ARPHRD_LOOPBACK;     // loopback = "lo"
    ifihdr->ifi_index = if_nametoindex("lo");
    ifihdr->ifi_change = 0;

    struct rtattr *rta_mtu = (void *)((char *)nlhdr + nlhdr->nlmsg_len);
    rta_mtu->rta_type = IFLA_MTU;
    *(int *)RTA_DATA(rta_mtu) = NEW_MTU;
    rta_mtu->rta_len = RTA_LENGTH(sizeof(int));

    nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)) + rta_mtu->rta_len;

    // send netlink message to kernel
    int n = sendto(sock, (void *)&nlhdr, nlhdr->nlmsg_len, 0, (struct sockaddr *)&sa, sizeof(sa));
    if (n < 0) {
        perror("sendto");
        return 1;
    }

    // receive reply message from kernel
    n = recv(sock, buf, sizeof(buf), 0);
    if (n < 0) {
        perror("recv msg");
        return 1;
    }
    printf("received netlink message\n");

    // analyze netlink message
    for (nlhdr = (struct nlmsghdr *)buf; NLMSG_OK(nlhdr, n); nlhdr = NLMSG_NEXT(nlhdr, n)) {
        printf("##########");
        printf("length: %d\n", nlhdr->nlmsg_len);
        printf("type  : %d\n", nlhdr->nlmsg_type);

        if (nlhdr->nlmsg_type == NLMSG_ERROR) {
            struct nlmsgerr *errmsg = NLMSG_DATA(nlhdr);
            printf("%d, %s\n", errmsg->error, strerror(errmsg->error));
        }
    } 

    close(sock);
    return 0;
}