#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <errno.h>

#define PORT_NUM 12345
#define NEVENTS 16
#define BUFSIZE 1024

enum state {    // 通信の state 管理のサンプル: この単純な echo server では未使用
    STATE_READ = 0,
    STATE_WRITE
};

struct clientinfo {
    int fd;
    char buf[1024];
    int n;
    int state;
};

int main() {
    // listen する socket を設定
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock0, 5);

    // epoll setting
    int epfd = epoll_create(NEVENTS);
    if (epfd < 0) {
        perror("epoll_create");
        return 1;
    }

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.ptr = malloc(sizeof(struct clientinfo));
    if (ev.data.ptr == NULL) {
        perror("malloc");
        return 1;
    }
    memset(ev.data.ptr, 0, sizeof(struct clientinfo));
    ((struct clientinfo *)ev.data.ptr)->fd = sock0;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock0, &ev) != 0) {
        perror("malloc");
        return 1;
    }

    // main loop
    struct epoll_event ev_ret[NEVENTS];
    struct sockaddr_in client;
    for (;;) {
        int nfds = epoll_wait(epfd, ev_ret, NEVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            return 1;
        }

        printf("after epoll_wait: nfds=%d\n", nfds);

        for (int i = 0; i < nfds; i++) {    // 準備ができている event 全体を走る
            struct clientinfo *ci = ev_ret[i].data.ptr;
            
            printf("fd=%d\n", ci->fd);

            if (ci->fd == sock0) {      // client からの新規接続要求 -> accept & epoll 登録
                socklen_t len = sizeof(sizeof client);
                int sock = accept(sock0, (struct sockaddr *)&client, &len);     // accept
                if (sock < 0) {
                    perror("accept");
                    return 1;
                }

                printf("accept sock=%d\n", sock);

                memset(&ev, 0, sizeof(ev));
                ev.events = EPOLLIN | EPOLLONESHOT;
                ev.data.ptr = malloc(sizeof(struct clientinfo));
                if (ev.data.ptr == NULL) {
                    perror("malloc");
                    return 1;
                }
                memset(ev.data.ptr, 0, sizeof(struct clientinfo));
                ((struct clientinfo *)ev.data.ptr)->fd = sock;

                if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) != 0) {       // epoll 登録
                    perror("epoll_ctl");
                    return 1;
                }
            } else {        // 登録済み socket
                if (ev_ret[i].events & EPOLLIN) {       // data: client -> server
                    // clientinfo: ci に受信データ書き込み
                    ci->n = read(ci->fd, ci->buf, BUFSIZE);
                    if (ci->n < 0) {
                        perror("read");
                        return 1;
                    }
                    ci->state = STATE_WRITE;

                    // socket が書き込み可能になった時点で event が発生するように epoll を再設定
                    ev_ret[i].events = EPOLLOUT;
                    if (epoll_ctl(epfd, EPOLL_CTL_MOD, ci->fd, &ev_ret[i]) != 0) {
                        perror("epoll_ctl");
                        return 1;
                    }
                } else if (ev_ret[i].events & EPOLLOUT) {   // data: server -> client
                    if (write(ci->fd, ci->buf, ci->n) < 0) {    // clientinfo: ci のデータを書き込む
                        perror("write");
                        return 1;
                    }

                    // 後処理: one_shot epoll を削除
                    if (epoll_ctl(epfd, EPOLL_CTL_DEL, ci->fd, &ev_ret[i]) != 0) {
                        perror("epoll_ctl");
                        return 1;
                    }
                    close(ci->fd);
                    free(ev_ret[i].data.ptr);
                }
            }
        }
    }   // 到達しない

    if (close(sock0) != 0) {
        perror("close");
        return 1;
    }
    return 0;
}