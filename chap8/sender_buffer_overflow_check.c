#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NEVENTS 16
#define ADDR "127.0.0.1"
#define PORT_NUM 12345

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, ADDR, &server.sin_addr.s_addr);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0) {
        perror("connect");
        return 1;
    }

    int epfd = epoll_create(NEVENTS);
    if (epfd < 0) {
        perror("epoll_create");
        return 1;
    }

    struct epoll_event ev, ev_ret[NEVENTS];
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLOUT;
    ev.data.fd = sock;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) != 0) {
        perror("epoll_ctl");
        return 1;
    }

    char buf[65536];
    while (1) {
        printf("before epoll_wait\n");

        int nfds = epoll_wait(epfd, ev_ret, NEVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            return 1;
        }

        printf("after epoll_wait\n");

        if (ev_ret[0].data.fd == sock) {
            printf("write %ld bytes\n", sizeof(buf));

            int n = write(sock, buf, sizeof(buf));
            if (n <= 0) {
                printf("write error: %d\n", n);
                break;
            }
        }
    }

    close(sock);
    return 0;
}