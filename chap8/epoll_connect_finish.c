#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <arpa/inet.h>

#define NEVENTS 16
#define ADDR_NOT_USED "192.168.0.0"
#define PORT_NUM 12345

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, ADDR_NOT_USED, &server.sin_addr.s_addr);

    int epfd = epoll_create(NEVENTS);
    if (epfd < 0) {
        perror("epoll_create");
        return 1;
    }

    struct epoll_event ev, ev_ret[NEVENTS];
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) != 0) {
        perror("epoll_ctl");
        return 1;
    }

    int val = 1;
    ioctl(sock, FIONBIO, &val);     // socket をノンブロッキングに設定

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0) {
        if (errno == EINPROGRESS) {     // connect() が (ノンブロッキングなので) 未完了
            printf("before epoll_wait\n");

            int nfds = epoll_wait(epfd, ev_ret, NEVENTS, 1000);     // connect() の完了を待つ
            if (nfds < 0) {
                perror("epoll_wait");
                return 1;
            }

            printf("after epoll_wait: nfds=%d\n", nfds);
        } else {
            perror("connect");
            return 1;
        }
    }
    
    char buf[32];
    int n = read(sock, buf, sizeof(buf));
    write(fileno(stdout), buf, n);

    close(sock);
    return 0;
}