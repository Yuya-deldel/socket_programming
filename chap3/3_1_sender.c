#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT_NUM 12345

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s dest_ip_address\n", argv[0]);
        return 1;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);     // convert "127.0.0.1" to binary

    // sendto(): write() の通信用拡張
    int n = sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (n < 1) {
        perror("sendto");
        return 1;
    }

    close(sock);
    return 0;
}