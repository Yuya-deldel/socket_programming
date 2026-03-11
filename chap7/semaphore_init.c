#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define NSEMS 16
#define SEM_INIT 1

int main() {
    int semid = semget(IPC_PRIVATE, NSEMS, 0600);
    if (semid < 0) {
        perror("semget");
        return 1;
    }

    // initialize semaphore
    unsigned short semun_array[NSEMS];
    for (int i = 0; i < NSEMS; i++) {
        semun_array[i] = SEM_INIT;
    }
    if (semctl(semid, NSEMS, SETALL, &semun_array) != 0) {
        perror("semctl");
        return 1;
    }

    printf("%d\n", semid);

    return 0;
}