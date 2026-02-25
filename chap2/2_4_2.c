#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main() {
    int err;
    if ((err = getaddrinfo(NULL, NULL, NULL, NULL)) != 0) {
        printf("error %d : %s\n", err, gai_strerror(err));
        return 1;
    }
    return 0;
}