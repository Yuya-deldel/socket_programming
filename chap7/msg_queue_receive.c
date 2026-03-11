#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MTEXTSIZE 10

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s msgid msgtype\n", argv[0]);
        return 1;
    }

    int msgid = atoi(argv[1]);
    int msgtype = atoi(argv[2]);
    struct msgbuf {
        long mtype;
        char mtext[MTEXTSIZE];
    } mbuf;

    if (msgrcv(msgid, &mbuf, MTEXTSIZE, msgtype, 0) < 0) {  // receive
        perror("msgrcv");
        return 1;
    }

    printf("%c\n", mbuf.mtext[0]);

    return 0;
}