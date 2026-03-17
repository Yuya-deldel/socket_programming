#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT_NUM "12345"
#define SSM_ADDR "232.1.2.3"    // 232.0.0.0/8: SSM

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s ssm_ipv4_addr\n", argv[0]);
        return 1;
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flag = AI_PASSIVE;
    int err = getaddrinfo(NULL, PORT_NUM, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    if (bind(sock, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind");
        return 1;
    }
    freeaddrinfo(res);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    err = getaddrinfo(SSM_ADDR, NULL, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    struct group_source_req gsr;
    memset(&gsr, 0, sizeof(gsr));
    gsr.gsr_interface = 0;      // any interface
    memcpy(&gsr.gsr_group, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    printf("SSM SOURCE: %s\n", argv[1]);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    err = getaddrinfo(argv[1], NULL, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    memcpy(&gsr.gsr_source, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    if (setsockopt(sock, IPPROTO_IP, MCAST_JOIN_SOURCE_GROUP, (void *)&gsr, sizeof(gsr)) != 0) {
        perror("setsockopt");
        return 1;
    }

    char buf[2048];
    if (recv(sock, buf, sizeof(buf), 0) < 1) {
        perror("recv");
        return 1;
    }

    printf("%s\n", buf);

    // leave from group
    if (setsockopt(sock, IPPROTO_IP, MCAST_LEAVE_SOURCE_GROUP, (void *)&gsr, sizeof(gsr)) != 0) {
        perror("setsockopt");
        return 1;
    }

    close(sock);
    return 0;
}