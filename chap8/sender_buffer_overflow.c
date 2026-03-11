#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT_NUM 12345

int main() {
    int sock0 = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

    listen(sock0, 5);

    struct sockaddr_in client;
    int len = sizeof(client);
    int sock = accept(sock0, (struct sockaddr *)&client, &len);

    printf("after accept\n");

    char buf[65536];
    for (int i = 0; i < 10; i++) {
        sleep(2);
        int n = read(sock, buf, sizeof(buf));
        printf("drain data [%d] bytes\n", n);
    }

    printf("close socket and finish\n");

    close(sock);
    close(sock0);
    return 0;
}