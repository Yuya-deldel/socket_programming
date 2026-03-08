#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define FILEPATH "/tmp/afunix_test"

int main() {
    int sock0 = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock0 < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un s_un, s_un_accept;
    s_un.sun_family = AF_UNIX;
    strcpy(s_un.sun_path, FILEPATH);
    
    if (bind(sock0, (struct sockaddr *)&s_un, sizeof(s_un)) != 0) {
        perror("bind");
        return 1;
    }

    if (listen(sock0, 5) != 0) {
        perror("listen");
        return 1;
    }

    socklen_t addrlen = sizeof(s_un_accept);
    int sock = accept(sock0, (struct sockaddr *)&s_un_accept, &addrlen);
    if (sock < 0) {
        perror("accept");
        return 1;
    }
    printf("after accept\n");

    write(sock, "HOGE", 4);

    close(sock);
    close(sock0);
    unlink(FILEPATH);
    return 0;
}