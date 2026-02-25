#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int sock_0 = socket(AF_INET, SOCK_STREAM, 0);       // socket 作成

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);           // int -> network bite order 変換
    addr.sin_addr.s_addr = INADDR_ANY;      // IP address

    bind(sock_0, (struct sockaddr *)&addr, sizeof(addr));   // socket にローカルアドレスを割り当てる
    listen(sock_0, 5);                      // 接続を受け入れる準備: backlog = 5 は接続待ち行列の長さ

    // client からの接続要求受け入れ: 接続待ちキューの先頭の接続要求に対し、接続された新規ソケット作成
    struct sockaddr_in client;
    int len = sizeof(client);
    int sock = accept(sock_0, (struct sockaddr *)&client, &len);

    // 送信
    write(sock, "HELLO", 5);

    close(sock);        // TCP session 終了
    close(sock_0);      // listen socket 終了
    return 0;
}