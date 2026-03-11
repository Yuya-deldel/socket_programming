#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    int shm = shmget(IPC_PRIVATE, 128, IPC_CREAT | 0600);
    if (shm < 0) {
        perror("shmget");
        return 1;
    }

    // attach shared memory 
    char *ptr = shmat(shm, NULL, 0);
    if ((int)ptr == -1) {
        perror("shmat");
        return 1;
    }

    // shared memory 書き込み
    strcpy(ptr, "HOGE");
    
    // detach shared memory
    shmdt(ptr);
    
    return 0;
}