#include <netinet/in.h>

typedef struct {
    struct sockaddr_in addr;
    int sockfd;
} MySocket;
