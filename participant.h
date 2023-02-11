#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>    
#include <stdlib.h>

#include <iostream>
#include <thread>
#include <string>

#include "socket.h"

class Participant {
private:
    MySocket sock_for_send;
    MySocket sock_for_get;
public:
    Participant();
    int getMessages();
    int sendMessages();
    int startChat();
};
