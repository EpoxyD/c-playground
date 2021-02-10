#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

static int sockfd = -1;

#define BUFSIZE 1024

static void setup_connection() {
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("error socket()\n");
        return;
    }

    struct sockaddr_un saddr = {AF_UNIX, "/tmp/zigbee.ipc"};
    bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));

    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        printf("error connect()\n");
        return;
    }
    printf("SAH IPC socket connected!\n");

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
}

static void receive_message(char *msg, int *msglen) {
    if (sockfd <= 0) {
        printf("error: SAH socket not initialized\n");
        return;
    }

    while (1) {
        *msglen = (int)read(sockfd, msg, BUFSIZE);
        if (*msglen > 0)
            return;
    }
}

static void *read_param(char *msg, int *offset, const int len) {
    void *result = (void *)&msg[*offset];
    *offset += len;
    return result;
}

static void *read_string(char *msg, int *offset) {
    int *len = (int *)&msg[*offset];
    *offset += sizeof(int);
    void *result = (void *)&msg[*offset];
    *offset += *len;
    return result;
}

static void parse_message(char *msg, int msglen) {
    if (strcmp(msg, "parameters") == 0) {
        printf("Found it\n");
        int offset = strlen(msg) + 1;
        int x = *(int *)read_param(msg, &offset, sizeof(int));
        char *str = (char *)read_string(msg, &offset);
        uint32_t sz = *(uint32_t *)read_param(msg, &offset, sizeof(int));
        printf("%d %s %u\n", x, str, sz);
    }
}

int main(void) {
    setup_connection();

    int msglen = -1;
    char msg[BUFSIZE];
    while (1) {
        receive_message(msg, &msglen);
        printf("Message = %s\n", msg);
        parse_message(msg, msglen);

        if (strcmp(msg, "end") == 0)
            break;
    }
}