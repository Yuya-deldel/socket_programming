#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MYIPCKEY 0xdeadbeef
#define NSEMS 16

int my_sem_init() {
    int i;
    unsigned short semun_array[NSEMS];

    pid_t pid = getpid();

    // IPC_EXcL: 二回目以降の同一 key の呼び出しは error: EEXIST
    int semid = semget(MYIPCKEY, NSEMS, 0600 | IPC_CREAT | IPC_EXCL);
    if (semid >= 0) {       // 初期化されていない
        // 初期化
        for (i = 0; i < NSEMS; i++) {
            semun_array[i] = 1;
        }
        if (semctl(semid, NSEMS, SETALL, &semun_array) != 0) {
            perror("semctl");
            return 1;
        }

        printf("[pid:%d] new semaphore et, semid=%d\n", pid, semid);

        struct sembuf sb[NSEMS];
        for (i = 0; i < NSEMS; i++) {
            sb[i].sem_num = i;
            sb[i].sem_op = -1;
            sb[i].sem_flg = SEM_UNDO;
        }

        printf("[pid:%d] before semop()\n", pid);
        printf("[pid:%d] press enter to start semop\n", pid);
        getchar();

        if (semop(semid, sb, NSEMS) != 0) {
            perror("semop");
            return 1;
        }

        printf("[pid:%d] press enter to exit this process\n", pid);
        getchar();

        exit(0);
    } else {        
        if (errno != EEXIST) {
            perror("semget");
            return 1;
        } else {    // 既に初期化されていた (errno = EEXIST)
            semid = semget(MYIPCKEY, NSEMS, 0600);
            if (semid < 0) {
                perror("semget");
                return 1;
            }

            printf("[pid:%d] before semctl\n", pid);

            // 初期化が終了するまで待つ
            struct semid_ds sds;
            for (;;) {
                if (semctl(semid, 0, IPC_STAT, &sds) != 0) {    // 情報 (sem_otime) 取得
                    perror("semctl");
                    return 1;
                }
                if (sds.sem_otime != 0) break;      // 初期化終了

                printf("[pid:%d] waiting otime change...\n", pid);
                sleep(2);
            }

            struct sembuf sb[NSEMS];
            sb[0].sem_num = 0;
            sb[0].sem_op = -1;
            sb[0].sem_flg = SEM_UNDO;

            printf("[pid:%d] before semop\n", pid);
            if (semop(semid, sb, 1) != 0) {
                perror("semop");
                return 1;
            }
            printf("[pid:%d] after semop\n", pid);
        }
    }
    return 0;
}

int main() {
    pid_t pid = fork();
    if (my_sem_init() < 0) {
        printf("[pid:%d] my_sem_init failed\n", getpid());
    }

    return 0;
}