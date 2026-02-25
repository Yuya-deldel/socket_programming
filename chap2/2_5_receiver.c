#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>

#define PORT_NUM "12345"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s outputfilename\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_WRONLY | O_CREAT, 0600);   // output 用 file 
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // res <- address 情報
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    int err = getaddrinfo(NULL, PORT_NUM, &hints, &res);
    if (err != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(err));
        return 1;
    }

    // create socket 
    int sock0 = socket(res->ai_family, res->ai_socktype, 0);
    if (sock0 < 0) {
        perror("socket");
        return 1;
    }

    // bind()
    if (bind(sock0, res->ai_addr, res->ai_addrlen) != 0) {
        perror("bind");
        return 1;
    }
    freeaddrinfo(res);

    // listen()
    listen(sock0, 5);  

    // accept()
    struct sockaddr_in client;
    int len = sizeof(client);
    int sock = accept(sock0, (struct sockaddr *)&client, &len);
    if (sock < 0) {
        perror("accept");
        return 1;
    }

    int n;
    char buf[65536];
    while ((n = read(sock, buf, sizeof(buf))) > 0) {
        if (write(fd, buf, n) < 1) {
            perror("write");
            break;
        }
    }

    close(sock);
    close(sock0);
    return 0;    
}