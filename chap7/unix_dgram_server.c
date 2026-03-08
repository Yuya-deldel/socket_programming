#include <stdio.h>
#include <string.h>
#include <unistd.h>
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
    
    int n = sendto(sock, "HOGE\n", 5, 0, (struct sockaddr *)&s_un, sizeof(s_un));

    printf("send data\n");

    close(sock);
    return 0;
}