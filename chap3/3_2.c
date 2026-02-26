#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT_NUM 12345

int main() {
    // create socket and set option for broadcasting 
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes)) != 0) {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    // 255.255.255.255: サブネット内のすべてのノードへのブロードキャスト
    inet_pton(AF_INET, "255.255.255.255", &addr.sin_addr.s_addr);   

    if (sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr)) < 1) {
        perror("sendto");
        return 1;
    }

    close(sock);
    return 0;
}