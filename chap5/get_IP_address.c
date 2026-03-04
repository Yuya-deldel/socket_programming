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
    sa.nl_groups = 0;    // unicast

    // create request message to kernel
    struct {
        struct nlmsghdr nh;
        struct ifaddrmsg ifa;
    } request;
    memset(&request, 0, sizeof(request));
    request.nh.nlmsg_len = sizeof(request);
    request.nh.nlmsg_type = RTM_GETADDR;
    request.nh.nlmsg_flags = NLM_F_ROOT | NLM_F_REQUEST;
    request.nh.nlmsg_pid = 0;       // to kernel
    request.nh.nlmsg_seq = MSG_SEQ + 1;
    request.ifa.ifa_family = AF_INET;

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
        printf("##########/n");
        printf("length: %d\n", nlhdr->nlmsg_len);
        printf("type  : %d\n", nlhdr->nlmsg_type);

        if (nlhdr->nlmsg_type != RTM_NEWADDR) continue;

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
            printf("  type = %d, len = %d\n", rta->rta_type, rta->rta_len);

            switch (rta->rta_type) {
                case IFA_ADDRESS:
                case IFA_BROADCAST:
                case IFA_LOCAL:
                    {
                        char str[128];
                        char *addrptr = RTA_DATA(rta);
                        inet_ntop(addrmsg->ifa_family, addrptr, str, sizeof(str));
                        printf("    + %s\n", str);
                    }
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

    close(sock);
    return 0;
}