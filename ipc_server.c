
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

static int sockfd = -1;
static int clientfd = -1;

#define SOCKFILE "/tmp/zigbee.ipc"
#define BUFSIZE 1024

#define SAH_TRACE_ERROR(...) printf(__VA_ARGS__);
#define SAH_TRACE_INFO(...) printf(__VA_ARGS__);

static void dump(char *msg) {
    for (int i = 0; i < 50; i++)
        printf("%c", msg[i]);
    printf("\n");
}

static void setup_connection(void) {
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        SAH_TRACE_ERROR("error socket()");
        return;
    }

    if (access(SOCKFILE, F_OK) != -1) {
        unlink(SOCKFILE);
    }

    struct sockaddr_un saddr = {AF_UNIX, SOCKFILE};
    if (bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        SAH_TRACE_ERROR("error bind()");
        return;
    }

    if (listen(sockfd, 1) < 0) {
        SAH_TRACE_ERROR("error listen()");
        return;
    }

    struct sockaddr_in caddr;       // client address
    socklen_t len = sizeof(caddr);  // address length could change
    clientfd = accept(sockfd, (struct sockaddr *)&caddr, &len);

    SAH_TRACE_INFO("SAH IPC socket connected!\n");

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
}

static bool send_message(char *msg, size_t msglen) {
    SAH_TRACE_INFO("sending msg: %s\n", msg);
    if (clientfd < 0) {
        SAH_TRACE_ERROR("socket not yet initialized\n");
        return false;
    }
    if (write(clientfd, msg, msglen) < 0) {
        SAH_TRACE_ERROR("error sending message");
        return false;
    }
    sleep(0.2);
    return true;
}

static void add_param(char *msg, int *offset, void *param, const int len) {
    memcpy(&msg[*offset], param, len);
    *offset += len;
}

static void add_string(char *msg, int *offset, char *str, const int len) {
    memcpy(&msg[*offset], &len, sizeof(int));
    *offset += sizeof(int);
    memcpy(&msg[*offset], str, len);
    *offset += len;
}

int main(void) {
    setup_connection();

    printf("%lu\n", sizeof(void));

    char message[] = "Hello World!";
    send_message(message, strlen(message) + 1);

    char command[] = "discattr";
    send_message(command, strlen(command) + 1);

    char msg[BUFSIZE] = "parameters";
    int offset = strlen(msg) + 1;
    int x = 512;
    char str[] = "Woo Hoo";
    uint32_t sz = 5678;
    add_param(msg, &offset, &x, sizeof(int));
    add_string(msg, &offset, str, strlen(str) + 1);
    add_param(msg, &offset, &sz, sizeof(uint32_t));
    dump(msg);
    send_message(msg, offset);

    char end[] = "end";
    send_message(end, strlen(end) + 1);
}