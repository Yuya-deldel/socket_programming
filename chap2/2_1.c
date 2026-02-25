#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

int main() {
    printf("fileno(stdin) = %d\n", fileno(stdin));
    close(0);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    printf("sock=%d\n", sock);

    return 0;
}