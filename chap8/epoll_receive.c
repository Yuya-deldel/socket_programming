#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define NEVENTS 16
#define ADDR "127.0.0.1"
#define PORT_NUM1 11111
#define PORT_NUM2 22222

int main() {
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    int sock2 = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in addr1, addr2;
    addr1.sin_family = AF_INET;
    addr2.sin_family = AF_INET;
    inet_pton(AF_INET, ADDR, &addr1.sin_addr.s_addr);
    inet_pton(AF_INET, ADDR, &addr2.sin_addr.s_addr);
    addr1.sin_port = htons(PORT_NUM1);
    addr2.sin_port = htons(PORT_NUM2);

    bind(sock1, (struct sockaddr *)&addr1, sizeof(addr1));
    bind(sock2, (struct sockaddr *)&addr2, sizeof(addr2));

    int epfd = epoll_create(NEVENTS);
    if (epfd < 0) {
        perror("epoll_create");
        return 1;
    }

    struct epoll_event ev, ev_ret[NEVENTS];
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = sock1;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock1, &ev) != 0) {
        perror("epoll_ctl");
        return 1;
    }

    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = sock2;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock2, &ev)) {
        perror("epoll_ctl");
        return 1;
    }

    char buf[2048];
    while (1) {
        printf("before epoll_wait\n");

        // sock1 or sock2 が受信可能となるまで待機
        int nfds = epoll_wait(epfd, ev_ret, NEVENTS, -1);
        if (nfds <= 0) {
            perror("epoll_wait");
            return 1;
        }

        printf("after epoll_wait\n");

        for (int i = 0; i < nfds; i++) {
            if (ev_ret[i].data.fd == sock1) {
                int n = recv(sock1, buf, sizeof(buf), 0);
                write(fileno(stdout), buf, n);
            } else if (ev_ret[i].data.fd = sock2) {
                int n = recv(sock2, buf, sizeof(buf), 0);
                write(fileno(stdout), buf, n);
            }
        }
    }   // 到達しない
    
    close(sock1);
    close(sock2);
    return 0;
}