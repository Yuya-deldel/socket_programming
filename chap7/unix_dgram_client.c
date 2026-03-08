#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define FILEPATH "/tmp/afunix_dgram"

int main() {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un s_un;
    s_un.sun_family = AF_UNIX;
    strcpy(s_un.sun_path, FILEPATH);

    char buf[2048];
    while (1) {
        memset(buf, 0, sizeof(buf));
        recv(sock, buf, sizeof(buf)-1, 0);

        printf("recv: %s\n", buf);
    }

    printf("%s\n", buf);

    close(sock);
    return 0;
}