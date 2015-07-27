#include "Client.h"

#define SECONDS_TO_RUN_CLIENT_DEFAULT   10

int atoi(const char *s) {
    int     result = 0;
    int     r = 0;
    while(*s != '\0') {
        if(*s < '0' || *s > '9')
            return SECONDS_TO_RUN_CLIENT_DEFAULT;

        r = *s - '0';
        result = 10 * result + r;
        ++s;    
    }

    return result;
}

int main(int argc, const char** argv) {
    /* For simple test, we assume server is localhost and port is 6323*/
    int     seconds = SECONDS_TO_RUN_CLIENT_DEFAULT; //arg4
    int     client_id = 1; //arg1
    char    server[31];  //arg2
    int     port = 6323;  //arg3
    
    strncpy(server, "localhost", 30);

    if(argc != 2) {
        std::cerr << "Wrong program argument input." << std::endl;
        return 1;
    }

    client_id = atoi(argv[1]);

    Client client(client_id, server, port);
    if(!client.Connect()) {
        std::cerr << "Client fails to connect to server." << std::endl;
        return 2;
    }

    client.Work(seconds);
    client.Disconnect();
    return 0;
}
