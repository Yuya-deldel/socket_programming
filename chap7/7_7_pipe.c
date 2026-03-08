#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    char buf[1024];
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
        printf("child process: id=%d\n", getpid());

        // stdout -> ファイル書き込み
        close(p[0]);
        dup2(p[1], fileno(stdout));

        if (execve("/bin/ls", NULL, NULL) < 0) {
            perror("exec");
            return 1;
        } 
    } else {
        close(p[1]);

        printf("parent process: child process id=%d\n", pid);

        FILE *filep = fdopen(p[0], "r");
        if (filep == NULL) {
            perror("fdopen");
            return 1;
        }

        while (fgets(buf, sizeof(buf), filep) != NULL) {
            printf("HOGE: %s", buf);
        }
    }

    return 0;
}