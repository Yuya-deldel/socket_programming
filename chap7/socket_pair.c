#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

int main() {
    char buf[128];
    memset(buf, 0, sizeof(buf));

    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        close(sv[1]);

        read(sv[0], buf, sizeof(buf));
        printf("child process: data from parent process [%s]\n", buf);

        exit(0);
    } else {
        close(sv[0]);

        write(sv[1], "HOGE", 4);
        printf("parent process: child process id [%d]\n", pid);

        int status;
        wait(&status);
    }

    return 0;
}