#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s shmid\n", argv[0]);
        return 1;
    }

    int shm = atoi(argv[1]);

    char *ptr = shmat(shm, NULL, 0);
    if ((int)ptr == -1) {
        perror("shmat");
        return 1;
    }

    printf("string from shared memory: %s\n", ptr);

    shmdt(ptr);

    return 0;
} 