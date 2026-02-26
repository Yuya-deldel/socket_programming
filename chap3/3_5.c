#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT_NUM 12345

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    // bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // データを受け取ると、同じデータを送り返す
    char buf[2048], senderstr[16];
    struct sockaddr_in senderinfo;
    while (1) {
        // receive
        memset(buf, 0, sizeof(buf));
        socklen_t addrlen = sizeof(senderinfo);
        int n = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr *)&senderinfo, &addrlen);

        // 送信元情報表示
        inet_ntop(AF_INET, &senderinfo.sin_addr, senderstr, sizeof(senderstr));
        printf("recvfrom: %s, port=%d\n", senderstr, ntohs(senderinfo.sin_port));

        // 返信
        sendto(sock, buf, n, 0, (struct sockaddr *)&senderinfo, addrlen);

        // 送信元情報再度表示
        printf("sent data to: %s, port=%d\n", senderstr, ntohs(senderinfo.sin_port));
    }   // ここには到達しない

    close(sock);
    return 0;
}