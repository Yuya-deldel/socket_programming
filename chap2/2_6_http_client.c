#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s dest\n", argv[0]);
        return 1;
    }

    // res0 <- address info
    struct addrinfo hints, *res0, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_UNSPEC;
    if (getaddrinfo(argv[1], "http", &hints, &res0) != 0) {
        printf("ERROR: %s\n", gai_strerror(errno));
        return 1;
    }

    // create socket & connect 
    int sock;
    for (res=res0; res!=NULL; res=res->ai_next) {
        printf("%d\n", res->ai_family);
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) continue;

        if (connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
            close(sock);
            continue;
        }

        break;
    }

    freeaddrinfo(res0);
    
    if (res == NULL) {
        fprintf(stderr, "failed to connect\n");
        return 1;
    }   // connect 成功

    // send request
    char buf[32];
    snprintf(buf, sizeof(buf), "GET / HTTP/1.0\r\n\r\n");
    int n = write(sock, buf, (int)strlen(buf));
    if (n < 0) {
        perror("write");
        return 1;
    }

    // receive request
    while (n > 0) {
        n = read(sock, buf, sizeof(buf));
        if (n < 0) {
            perror("read");
            return 1;
        }

        write(fileno(stdout), buf, n);
    }

    close(sock);
    return 0;
}