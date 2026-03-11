#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/wait.h>

int main() {
    // mutex 用の共有メモリ確保
    int shmid = shmget(IPC_PRIVATE, sizeof(pthread_mutex_t), 0600);
    if (shmid < 0) {
        perror("shmget");
        return 1;
    }

    pthread_mutex_t *m = shmat(shmid, NULL, 0);
    if ((int)m == -1) {
        perror("shmat");
        return 1;
    }

    // mutex の設定
    pthread_mutexattr_t mat;
    pthread_mutexattr_init(&mat);
    if (pthread_mutexattr_setpshared(&mat, PTHREAD_PROCESS_SHARED) != 0) {
        perror("pthread_mutexattr_setpshared");
        return 1;
    }
    pthread_mutex_init(m, &mat);

    pid_t pid = fork();

    printf("[%s] before pthread_mutex_lock\n", pid == 0 ? "child" : "parent");
    
    if (pthread_mutex_lock(m) != 0) {       // lock
        perror("pthread_mutex_lock");
        return 1;
    }

    printf("[%s] press enter\n", pid == 0 ? "child" : "parent");
    getchar();

    if (pthread_mutex_unlock(m) != 0) {     // unlock
        perror("pthread_mutex_unlock");
        return 1;
    }

    printf("[%s] after pthread_mutex_unlock\n", pid == 0 ? "child" : "parent");

    shmdt(m);

    if (pid != 0) {     // child process の終了を待つ
        wait(NULL);
        printf("[%s] after wait\n", pid == 0 ? "child" : "parent");
        
        // 共有メモリ削除
        if (shmctl(shmid, IPC_RMID, NULL) != 0) {
            perror("shmctl");
            return 1;
        }
    }

    return 0;
}