#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MTEXTSIZE 10
#define MSGTYPE 777

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s msgid\n", argv[0]);
        return 1;
    }

    int msgid = atoi(argv[1]);

    struct msgbuf {
        long mtype;
        char mtext[MTEXTSIZE];
    } mbuf;
    mbuf.mtype = MSGTYPE;
    memset(&mbuf.mtext, 0, MTEXTSIZE);
    mbuf.mtext[0] = 'B';

    if (msgsnd(msgid, &mbuf, MTEXTSIZE, 0) != 0) {
        perror("msgsnd");
        return 1;
    }

    return 0;
}