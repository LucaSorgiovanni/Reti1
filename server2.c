#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

int serverFunc(int);

int main(int argc, char *argv[]) {

    int simpleSocket = 0;
    int simplePort = 0;
    int returnStatus = 0;
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
        int closing;

	/* wait here */

        simpleChildSocket = accept(simpleSocket,(struct sockaddr *)&clientName, &clientNameLength);
        if (simpleChildSocket == -1) {
            fprintf(stderr, "Cannot accept connections!\n");
            close(simpleSocket);
            exit(1);
        }
        /* handle the new connection request  */
        closing = serverFunc(simpleChildSocket);
        close(simpleChildSocket);

    }

    close(simpleSocket);
    return 0;

}

int serverFunc(int simpleChildSocket){

    char Benvenuto[] = "OK START Benvenuto, mandami i tuoi dati.\n";
    int returnStatus = 0;
    int indice;
    int totaleNumeri = 0;
    long valori[128] = {0};
    char buffer[512] = "";
    long numero;
    char *ptr;
    char *buff;
    float media = 0;
    float varianza = 0;

    /* write out our message to the client */

    write(simpleChildSocket, Benvenuto, strlen(Benvenuto));
    while(1){
        returnStatus = read(simpleChildSocket, buffer, sizeof(buffer));

        if ( returnStatus > 0 ) {
            buff = buffer;
            numero = strtol(buff, &ptr, 10);

            if (ptr == buff){
                memset(buffer, '\0', sizeof(buffer));
                write(simpleChildSocket, "ERR SYNTAX Errore sintattico riprovare!\n", strlen("ERR SYNTAX Errore sintattico riprovare!\n"));
                return -1;
            }
            buff = ptr;
            indice = numero;

        } else {
            fprintf(stderr, "Return Status = %d \n", returnStatus);
            return -4;
        }

        if (indice == 0 && totaleNumeri != 0){
            for (int i = 0; i < totaleNumeri; ++i){
                media = media + valori[i];
            }  
            media = media/totaleNumeri;

            for (int i = 0; i < totaleNumeri; ++i){
            varianza = varianza + (valori[i]-media)*(valori[i]-media);
            }
            varianza = varianza/totaleNumeri;

            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "OK STATS %d %.3f %.3f\n", totaleNumeri, media, varianza);
            write(simpleChildSocket, buffer, strlen(buffer));
            return 0;

        } else if (indice == 0 && totaleNumeri == 0){
            memset(buffer, '\0', sizeof(buffer));
            write(simpleChildSocket, "ERR STATS Impossibile calcolare\n" , strlen("ERR STATS Impossibile calcolare\n"));
            return -2;

        } else {
            totaleNumeri = totaleNumeri + indice;
            for (int i = 0; *buff != '\n' && i < indice; ++i){
                numero = strtol(buff, &ptr, 10);
                valori[totaleNumeri-indice+i] = numero;
                if (ptr == buff){
                    memset(buffer, '\0', sizeof(buffer));
                    write(simpleChildSocket, "ERR SYNTAX Errore sintattico riprovare!\n", strlen("ERR SYNTAX Errore sintattico riprovare!\n"));   
                    return -1;
                }
                buff = ptr;
            }

            if (*buff != '\n'){
                memset(buffer, '\0', sizeof(buffer));
                write(simpleChildSocket, "ERR DATA Errore semantico riprovare!\n", strlen("ERR DATA Errore semantico riprovare!\n")); 
                return -3;
            }
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "OK DATA %d\n", indice);
            write(simpleChildSocket, buffer, strlen(buffer));

            memset(buffer, 0, sizeof(buffer));
        }
    }
}

