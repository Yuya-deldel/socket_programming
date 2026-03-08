#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define FILEPATH "/tmp/afunix_test"

int main() {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un s_un;
    s_un.sun_family = AF_UNIX;
    strcpy(s_un.sun_path, FILEPATH);

    if (connect(sock, (struct sockaddr *)&s_un, sizeof(s_un)) != 0) {
        perror("connect");
        return 1;
    }
    printf("after connect\n");

    char buf[128];
    memset(buf, 0, sizeof(buf));
    if (read(sock, buf, sizeof(buf)) < 0) {
        perror("read");
        return 1;
    }

    printf("%s\n", buf);

    close(sock);
    return 0;
}