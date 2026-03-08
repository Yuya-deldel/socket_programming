#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    char buf[128];
    memset(buf, 0, sizeof(buf));

    int p[2];
    if (pipe(p) != 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        close(p[1]);

        read(p[0], buf, sizeof(buf));
        printf("child process: data from parent process [%s]\n", buf);
    } else {
        close(p[0]);

        write(p[1], "HOGE", 4);
        printf("parent process: child process id [%d]\n", pid);
    }

    return 0;
}