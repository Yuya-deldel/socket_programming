#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 12345

int main() {
    // create IPv4 UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    // bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // recvfrom()
    char buf[2048];
    struct sockaddr_in senderinfo;
    socklen_t addrlen = sizeof(senderinfo);
    int n = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr *)&senderinfo, &addrlen);

    // program
    write(fileno(stdout), buf, n);

    close(sock);
    return 0;
}