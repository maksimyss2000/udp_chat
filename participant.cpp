#include "participant.h"
#include <string.h>     /* for memset() */

Participant::Participant() {
    memset(&sock_for_get.addr, 0, sizeof(sock_for_get.addr)); 
    sock_for_get.addr.sin_family      = AF_INET;    /* IPv4 */
    sock_for_get.addr.sin_port        = htons(500);
    sock_for_get.addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&sock_for_send.addr, 0, sizeof(sock_for_send.addr)); 
    sock_for_send.addr.sin_family      = AF_INET;    /* IPv4 */
    sock_for_send.addr.sin_port        = htons(80);
    sock_for_send.addr.sin_addr.s_addr = inet_addr("255.255.255.255");
    
}

int Participant::getMessages() {
    if ( (sock_for_get.sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
    }
    
    int broadcastEnable=1;
    int ret=setsockopt(sock_for_get.sockfd , SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret) {
        perror("Error: Could not open set socket to broadcast mode");
    }

        int reuse = 1;
    if (setsockopt(sock_for_get.sockfd , SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    #ifdef SO_REUSEPORT
    if (setsockopt(sock_for_get.sockfd , SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        perror("setsockopt(SO_REUSEPORT) failed");
    #endif
    
    if ( bind(sock_for_get.sockfd , (const struct sockaddr *)&sock_for_get.addr, 
            sizeof(sock_for_get.addr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    int MAXRECVSTRING = 260;
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast Address */
    unsigned short broadcastPort;     /* Port */
    char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */
    broadcastPort = 80;   /* First arg: broadcast port */
    
    
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        perror("socket() failed");


        int broadcastEnable=1;
    int ret=setsockopt(sock , SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret) {
        perror("Error: Could not open set socket to broadcast mode");
    }

        int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    #ifdef SO_REUSEPORT
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        perror("setsockopt(SO_REUSEPORT) failed");
    #endif
        memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */
    broadcastAddr.sin_port = htons(broadcastPort);      /* Broadcast port */

    /* Bind to the broadcast port */
    if (bind(sock, (struct sockaddr *) &broadcastAddr, sizeof(broadcastAddr)) < 0)
        perror("bind() failed");

    /* Receive a single datagram from the server */
    while (true)
    {
    
        if ((recvStringLen = recvfrom(sock, recvString, MAXRECVSTRING, 0, NULL, 0)) < 0)
            perror("recvfrom() failed");
        recvString[recvStringLen+1] = '\0';
        printf("Received: %s\n", recvString);    /* Print the received string */
    }
    /*
    *size_t bufSize = 1024;
    *char *buf = (char*) malloc(bufSize);
    *socklen_t len;  //len is value/result
    *
    *
    *while (true) {
    *    ssize_t result = recvfrom(sock_for_get.sockfd , buf, bufSize, 0, (struct sockaddr *)&sock_for_get.addr, &len);
    *    buf[len] = '\0';
    *    std::cout <<  buf <<std::endl;
    *}
    *return 0;
    */
}   

int Participant::sendMessages() {
    if ((sock_for_send.sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        std::exit(EXIT_FAILURE);
    }
    int broadcastEnable=1;
    int ret=setsockopt(sock_for_send.sockfd , SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret) {
        perror("Error: Could not open set socket to broadcast mode");
    }
    ret=setsockopt(sock_for_send.sockfd , SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    if (ret) {
        perror("Error: Could not open set socket to broadcast mode");
    }
    char netif[] = "lo";
    setsockopt(sock_for_send.sockfd , SOL_SOCKET, SO_BINDTODEVICE, netif, sizeof(netif));
    std::string message;
    while (std::getline(std::cin, message)) {
    sendto(sock_for_send.sockfd, (const char *)message.c_str(), message.size(),
            MSG_CONFIRM, (const struct sockaddr *) &sock_for_send.addr,                    /* MSG_CONFIRM?? */
            sizeof(sock_for_send.addr));
    }
    return 0;
}

int Participant::startChat() {
    std::thread first(&Participant::getMessages, this);
    std::thread second(&Participant::sendMessages, this);
    first.join();   
    second.join();
    return 0;

}
