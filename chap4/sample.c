#include <stdio.h>
#include <string.h>
#include <net/if.h>

int main() {
    int index = if_nametoindex("lo");
    if (index == 0) {
        perror("if_nametoindex");
        return 1;
    }
    printf("interface index: %d\n", index);

    char buf[128];
    memset(buf, 0, sizeof(buf));
    if (if_indextoname(index, buf) == NULL) {
        perror("if_indextoname");
        return 1;
    }
    printf("name: %s\n", buf);

    return 0;
}