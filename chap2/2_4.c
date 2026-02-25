#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// 名前解決: www.example.com -> IP address (IPv4)
int main() {
    char *hostname = "localhost";

    struct addrinfo hints;
    memset(&hints, 0, sizeof(sizeof(hints)));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    struct addrinfo *result;
    int err;
    if ((err = getaddrinfo(hostname, NULL, &hints, &result)) != 0) {
        printf("error %d\n", err);
        return 1;
    }

    struct in_addr addr;
    addr.s_addr = ((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr;

    // ?: inet_ntoa() の型: man によれば char が返されるはずだが実際には int が返される
    printf("IP address: %d\n", inet_ntoa(addr));

    freeaddrinfo(result);   // getaddrinfo はデータをヒープ上に確保 -> free() が必要
    
    return 0;
}