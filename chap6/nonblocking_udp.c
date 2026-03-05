#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <errno.h>

#define PORT_NUM 12345

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    int val = 1;    // non-blocking mode: val = 1, blocking mode: val = 0
    ioctl(sock, FIONBIO, &val);

    char buf[2048];
    while (1) {
        memset(buf, 0, sizeof(buf));

        int n = recv(sock, buf, sizeof(buf), 0);
        if (n < 1) {
            if (errno == EAGAIN) {
                printf("try again\n");
            } else {
                perror("recv");
                break;
            }
        } else {
            printf("received data\n");
            printf("%s\n", buf);
            break;
        }

        sleep(1);
    }

    close(sock);
    return 0;
}