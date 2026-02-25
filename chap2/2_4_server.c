#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT_NUM "12345"

int main() {
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;        // AI_PASSIVE flag に関しては man getaddrinfo を参照
    hints.ai_socktype = SOCK_STREAM;
    
    struct addrinfo *result;
    int err = getaddrinfo(NULL, PORT_NUM, &hints, &result);     // result に情報を格納
    if (err != 0) {
        printf("getaddrinfo : %s\n", gai_strerror(err));
        return 1;
    }

    int sock_0 = socket(result->ai_family, result->ai_socktype, 0);     // socket 作成
    if (sock_0 < 0) {
        perror("socket");
        return 1;
    }

    if (bind(sock_0, result->ai_addr, result->ai_addrlen) != 0) {       // bind()
        perror("bind");
        return 1;
    }
    
    freeaddrinfo(result);

    listen(sock_0, 5);

    struct sockaddr_in client;
    int len = sizeof(client);
    int sock = accept(sock_0, (struct sockaddr *)&client, &len);

    /* ... program ... */
    write(sock, "HELLO", 5);

    close(sock);        // TCP session 終了
    close(sock_0);      // listen socket 終了
    return 0;
}