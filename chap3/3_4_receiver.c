#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT_NUM "12345"

int main() {
    // res <- address info
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    int err = getaddrinfo(NULL, PORT_NUM, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    // create socket
    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // bind (setsockopt() を実行するには前もって bind() しておかなければならない)
    if (bind(sock, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind");
        return 1;
    }

    // multi-cast group に join する
    // res <- address info
    freeaddrinfo(res);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    err = getaddrinfo("239.192.1.2", NULL, &hints, &res);   // multi-cast address
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    // join のためのデータ準備
    struct group_req greq;
    memset(&greq, 0, sizeof(greq));
    greq.gr_interface = 0;      // 任意のネットワークインターフェースを使用
    memcpy(&greq.gr_group, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    // join
    if (setsockopt(sock, IPPROTO_IP, MCAST_JOIN_GROUP, (char *)&greq, sizeof(greq)) != 0) {
        perror("setsockopt");
        return 1;
    }

    // receive
    char buf[2048];
    memset(buf, 0, sizeof(buf));
    recv(sock, buf, sizeof(buf), 0);

    // program
    printf("%s\n", buf);

    close(sock);
    return 0;
}