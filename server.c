#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 


const char MESSAGE[] = "OK Start Benvenuto, mandami i tuoi dati.\n";

int main(int argc, char *argv[]) {

    int simpleSocket = 0;
    int simplePort = 0;
    int returnStatus = 0;
    char buffer[512] = "";
    struct sockaddr_in simpleServer;

    if (2 != argc) {

        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);

    }

    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (simpleSocket == -1) {

        fprintf(stderr, "Could not create a socket!\n");
        exit(1);

    }
    else {
	    fprintf(stderr, "Socket created!\n");
    }

    /* retrieve the port number for listening */
    simplePort = atoi(argv[1]);

    /* setup the address structure */
    /* use INADDR_ANY to bind to all local addresses  */
    memset(&simpleServer, '\0', sizeof(simpleServer)); 
    simpleServer.sin_family = AF_INET;
    simpleServer.sin_addr.s_addr = htonl(INADDR_ANY);
    simpleServer.sin_port = htons(simplePort);

    /*  bind to the address and port with our socket  */
    returnStatus = bind(simpleSocket,(struct sockaddr *)&simpleServer,sizeof(simpleServer));

    if (returnStatus == 0) {
	    fprintf(stderr, "Bind completed!\n");
    }
    else {
        fprintf(stderr, "Could not bind to address!\n");
	close(simpleSocket);
	exit(1);
    }

    /* lets listen on the socket for connections      */
    returnStatus = listen(simpleSocket, 5);

    if (returnStatus == -1) {
        fprintf(stderr, "Cannot listen on socket!\n");
	close(simpleSocket);
        exit(1);
    }

    while (1){
        struct sockaddr_in clientName = { 0 };
        int simpleChildSocket = 0;
        int clientNameLength = sizeof(clientName);

	/* wait here */

        simpleChildSocket = accept(simpleSocket,(struct sockaddr *)&clientName, &clientNameLength);
        if (simpleChildSocket == -1) {
            fprintf(stderr, "Cannot accept connections!\n");
            close(simpleSocket);
            exit(1);
        }
        /* handle the new connection request  */

        /* write out our message to the client */

        write(simpleChildSocket, MESSAGE, strlen(MESSAGE));

        returnStatus = read(simpleChildSocket, buffer, sizeof(buffer));

        if ( returnStatus > 0 ) {
            printf("Numeri: %s\n", buffer);
        } else {
            fprintf(stderr, "Return Status = %d \n", returnStatus);
        }

        char *token;
        int indice;
        int valori[128];

        token = strtok(buffer," ");
        printf("%s\n", token);
        indice = atoi(token);
        printf("Indice:%d\n", indice);
        printf("baubau\n");
        for (int i = 0; token != NULL; ++i){
            token = strtok(NULL," ");
            if (token != NULL) {
                valori[i] = atoi(token);
            }
        }
        printf("miao\n");

        for (int i = 0; i < indice; ++i){
            printf("%d \n",valori[i]);
        }

        memset(buffer, 0, sizeof(buffer));
        close(simpleChildSocket);

    }

    close(simpleSocket);
    return 0;

}

