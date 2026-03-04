#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/rtnetlink.h>

#define MSG_SEQ 100

int main() {
    // create netlink socket
    int sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

    // create sockaddr_nl structure
    struct sockaddr_nl sa;
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_pid = 0;       // kernel
    sa.nl_groups = RTMGRP_IPV4_IFADDR;  // IPv4 address に変更があると通知が送られるマルチキャストグループ

    // マルチキャストグループに bind()
    if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
        perror("bind");
        return 1;
    }

    // loop to receive
    char buf[4096];
    struct nlmsghdr *nlhdr;
    while (1) {
        // receive message from kernel
        printf("ready to receive\n");
        int n = recv(sock, buf, sizeof(buf), 0);
        if (n < 0) {
            perror("recv msg");
            return 1;
        }
        printf("received netlink message\n");

        for (nlhdr = (struct nlmsghdr *)buf; NLMSG_OK(nlhdr, n); nlhdr = NLMSG_NEXT(nlhdr, n)) {
            printf("##########/n");
            printf("length: %d\n", nlhdr->nlmsg_len);
            printf("type  : %d\n", nlhdr->nlmsg_type);

            if (nlhdr->nlmsg_type != RTM_NEWADDR && nlhdr->nlmsg_type != RTM_DELADDR) continue;

            // print address info
            struct ifaddrmsg *addrmsg = NLMSG_DATA(nlhdr);
            printf("  family   : %d\n", addrmsg->ifa_family);
            printf("  prefixlen: %d\n", addrmsg->ifa_prefixlen);
            printf("  flags    : %d\n", addrmsg->ifa_flags);
            printf("  scope    : %d\n", addrmsg->ifa_scope);
            printf("  index    : %d\n", addrmsg->ifa_index);

            n = nlhdr->nlmsg_len - NLMSG_LENGTH(sizeof(struct ifaddrmsg));
            struct rtattr *rta;
            for (rta = IFA_RTA(addrmsg); RTA_OK(rta, n); rta = RTA_NEXT(rta, n)) {
                printf("    type = %d, len = %d\n", rta->rta_type, rta->rta_len);

                switch (rta->rta_type) {
                    case IFA_ADDRESS:
                    case IFA_BROADCAST:
                    case IFA_LOCAL:
                        char str[48];
                        char *addrptr = RTA_DATA(rta);
                        inet_ntop(addrmsg->ifa_family, addrptr, str, sizeof(str));
                        printf("    + %s\n", str);

                        break;
                
                    case IFA_LABEL:
                        printf("    + %s\n", (char *)RTA_DATA(rta));
                        break;

                    default:
                        printf("    other type\n");
                        break;
                }
            }
        }
    }   // 到達しない

    close(sock);
    return 0;
}