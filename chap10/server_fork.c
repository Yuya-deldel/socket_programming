#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>

#define PORT_NUM 12345

int main() {
    int sock0 = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock0, 5);

    struct sockaddr_in client;
    char buf[1024];
    for (;;) {
        socklen_t len = sizeof(client);
        int sock = accept(sock0, (struct sockaddr *)&client, &len);     // client からの接続要求

        pid_t pid = fork();
        if (pid == 0) {     
            // child process (echo server)
            int n = read(sock, buf, sizeof(buf));
            if (n < 0) {
                perror("read");
                return 1;
            }

            write(sock, buf, n);

            close(sock);
            return 0;
        } else {
            int status;
            pid_t cpid;
            while ((cpid = waitpid(-1, &status, WNOHANG)) > 0) {
                /*  -1: どれか一つの child process が終了するまで待つ
                    WNOHANG: non-blocking: 状態が変化した child process がなかった場合すぐ復帰する

                    child process がなくなると cpid < 0 && errno == ECHILD となる
                */
                if (cpid < 0 && errno != ECHILD) {
                    perror("waitpid");
                    return 1;
                }   // child process がなくなった
            }
        }
    }   // 到達しない

    close(sock0);
    return 0;
}