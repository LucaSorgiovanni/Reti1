#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int clientFunc(int);

int main(int argc, char *argv[]) {

    int simpleSocket = 0;
    int simplePort = 0;
    int returnStatus = 0;
    int returnClientClose = -1;
    struct sockaddr_in simpleServer;

    if (3 != argc) {

        fprintf(stderr, "Usage: %s <server> <port>\n", argv[0]);
        exit(1);

    }

    /* create a streaming socket      */
    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (simpleSocket == -1) {

        fprintf(stderr, "Could not create a socket!\n");
        exit(1);

    }
    else {
	    fprintf(stderr, "Socket created!\n");
    }

    /* retrieve the port number for connecting */
    simplePort = atoi(argv[2]);

    /* setup the address structure */
    /* use the IP address sent as an argument for the server address  */
    //bzero(&simpleServer, sizeof(simpleServer)); 
    memset(&simpleServer, '\0', sizeof(simpleServer));
    simpleServer.sin_family = AF_INET;
    //inet_addr(argv[2], &simpleServer.sin_addr.s_addr);
    simpleServer.sin_addr.s_addr=inet_addr(argv[1]);
    simpleServer.sin_port = htons(simplePort);

    /*  connect to the address and port with our socket  */
    returnStatus = connect(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));

    if (returnStatus == 0) {
	    fprintf(stderr, "Connect successful!\n");
    }
    else {
        fprintf(stderr, "Could not connect to address!\n");
        close(simpleSocket);
        exit(1);
    }

    returnClientClose = clientFunc(simpleSocket);

    close(simpleSocket);
    return 0;

}

int clientFunc(int simpleSocket){
    int returnStatus = 0;
    char buffer[512] = "";
    char *tmp;

    returnStatus = read(simpleSocket, buffer, sizeof(buffer));

    if ( returnStatus > 0 ) {
        if (strncmp(buffer, "OK START", strlen("OK START")) == 0){
            tmp = buffer + strlen("OK START") + 1;
            printf("%s\n", tmp);
            printf("Questo programma permette di calcolare la media e la varianza dei numeri inseriti\nI numeri vanno inseriti con uno spazio che li separa e preceduti dal numero di numeri da inserire\nESEMPIO: 3 10 20 30\nPer calcolare la media e la varianza dopo aver inserito i numeri basta inserire solo lo zero (0) e premere invio\n");
        }
    } else {
        fprintf(stderr, "Return Status = %d \n", returnStatus);
        return -2;
    }
    memset(buffer, '\0', sizeof(buffer));

    //char buff[512];
    //char stringa[512];
    while(1){
        printf("Inserisci i dati nelle modalità indicate precedentemente oppure zero (0) per ottenere il risultato\n");
        fgets(buffer, sizeof(buffer), stdin);
        int indice = 0;
        int numero = 0;
        char buffcpy[512] = "";
        char *buff;
        char *ptr;
        strcpy(buffcpy, buffer);
        buff = buffcpy;
        numero = strtol(buff, &ptr, 10);
       

        if (ptr == buff){
            printf("ERR SYNTAX Errore sintattico riprovare!\n");
            return -1;
        }

        buff = ptr;
        indice = numero;

        for (int i = 0; *buff != '\n' && i < indice; ++i){
            numero = strtol(buff, &ptr, 10);
            if (ptr == buff){
                printf("ERR SYNTAX Errore sintattico riprovare!\n");
                return -1;
            }
            buff = ptr;
        }

        write(simpleSocket, buffer, strlen(buffer));
        memset(buffer, '\0', sizeof(buffer));

        returnStatus = read(simpleSocket, buffer, sizeof(buffer));
        if ( returnStatus > 0 ) {
            if (strncmp(buffer, "OK STATS", strlen("OK STATS")) == 0){
                int numeri = 0;
                float varianza = 0;
                float media = 0;
                tmp = strtok(buffer, " ");
                tmp = strtok(NULL, " ");
                tmp = strtok(NULL, " ");
                numeri = strtol(tmp, NULL, 10);
                tmp = strtok(NULL, " ");

                printf("Numeri: %d\n", numeri);
                media = strtof(tmp, NULL);
                tmp = strtok(NULL, " ");
                printf("Media: %.3f\n", media);
                varianza = strtof(tmp, NULL);
                printf("Varianza: %.3f\n", varianza);
                memset(buffer, 0, sizeof(buffer));
                return 0;
            }

            if (strncmp(buffer, "OK DATA", strlen("OK DATA")) == 0){
                int numeri = 0;
                tmp = strtok(buffer, " ");
                tmp = strtok(NULL, " ");
                tmp = strtok(NULL, " ");
                numeri = strtol(tmp, NULL, 10);
                memset(buffer, 0, sizeof(buffer));
            }

            if (strncmp(buffer, "ERR DATA", strlen("ERR DATA")) == 0){
                tmp = buffer + strlen("ERR DATA") + 1;
                printf("%s\n",tmp);
                memset(buffer, 0, sizeof(buffer));
                return -1;

            }
            if (strncmp(buffer, "ERR STATS", strlen("ERR STATS")) == 0){
                tmp = buffer + strlen("ERR STATS") + 1;
                printf("%s\n",tmp);
                memset(buffer, 0, sizeof(buffer));
                return -1;
            }

            if (strncmp(buffer, "ERR SYNTAX", strlen("ERR SYNTAX")) == 0){
                tmp = buffer + strlen("ERR SYNTAX") + 1;
                printf("%s\n",tmp);
                memset(buffer, 0, sizeof(buffer));
                return -1;
            }

        } else {
            fprintf(stderr, "Return Status = %d \n", returnStatus);
            return -2;
        }

        
    }
}

