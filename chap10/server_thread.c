#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>

#define PORT_NUM 12345

struct clientdata {
    int sock;
    struct sockaddr_in saddr;
};

void *threadfunc(void *data) {
    if (data == NULL) return (void *)-1;
    
    struct clientdata *cdata = data;
    int sock = cdata->sock;

    char buf[1024];
    int n = read(sock, buf, sizeof(buf));
    if (n < 0) {
        perror("read");
        goto err;
    }

    n = write(sock, buf, n);
    if (n < 0) {
        perror("write");
        goto err;
    }

    // end of TCP session
    if (close(sock) != 0) {
        perror("close");
        goto err;
    }

    free(data);
    return NULL;

    err:
        free(data);
        return (void *)-1;
}

int main() {
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock0, 5);

    char buf[1024];
    for (;;) {
        struct clientdata *cdata = malloc(sizeof(struct clientdata));
        if (cdata == NULL) {
            perror("malloc");
            return 1;
        }

        socklen_t len = sizeof(cdata->saddr);
        int sock = accept(sock0, (struct sockaddr *)&cdata->saddr, &len);     // client からの接続要求

        pthread_t th;
        if (pthread_create(&th, NULL, threadfunc, cdata) != 0) {
            perror("pthread_create");
            return 1;
        }
        
        if (pthread_detach(th) != 0) {      // parent thread の join を child process 側で行う
            perror("pthread_detach");
            return 1;
        }
    }   // 到達しない

    if (close(sock0) != 0) {
        perror("close");
        return 1;
    }
    return 0;
}