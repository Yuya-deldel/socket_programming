#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();
    if (pid == 0) {
        printf("child process: %d\n", getpid());
        exit(0);
    } else {
        printf("parent process: child_pid=%d, my_pid=%d\n", pid, getpid());

        int status;
        pid_t waitpid = wait(&status);
        printf("waitpid: %d\n", waitpid);
    }

    return 0;
}