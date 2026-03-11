#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s shmid\n", argv[0]);
        return 1;
    }

    int shm = atoi(argv[1]);

    struct shmid_ds sds;
    if (shmctl(shm, IPC_RMID, &sds) != 0) {
        perror("shmctl");
        return 1;
    }

    return 0;
}