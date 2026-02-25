#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>

#define PORT_NUM "12345"

int main(int argc, char *argv[]) {
    int err, sock, n;
    char buf[65536];
    struct addrinfo hints, *res0, *res;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s hostname filename\n", argv[0]);
        return 1;
    }

    int fd = open(argv[2], O_RDONLY);   // file open
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // res0 <- アドレス情報
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_UNSPEC;
    if ((err = getaddrinfo(argv[1], PORT_NUM, &hints, &res0)) != 0) {
        printf("error %d: %s\n", err, gai_strerror(err));
        return 1;
    }

    // res での connect() を試す
    for (res=res0; res!=NULL; res=res->ai_next) {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) continue;

        if (connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
            close(sock);
            continue;
        }

        break;
    }
    
    freeaddrinfo(res0);

    if (res == NULL) {      // connect 失敗
        printf("failed to connect\n");
        return 1;
    }   // 以下 connect() 成功

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (write(sock, buf, n) < 1) {
            perror("write");
            break;
        }
    }

    close(sock);
    return 0;
}