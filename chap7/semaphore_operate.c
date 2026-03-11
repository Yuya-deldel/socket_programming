#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define NSEMS 16

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s semid\n", argv[0]);
        return 1;
    }

    int semid = atoi(argv[1]);

    // semaphore 操作の内容を定める
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;
    
    printf("before semop\n");
    if (semop(semid, &sb, 1) != 0) {        // 操作
        perror("semop");
        return 1;
    }
    printf("after semop\n");

    printf("press enter to exit\n");
    getchar();

    return 0;
}