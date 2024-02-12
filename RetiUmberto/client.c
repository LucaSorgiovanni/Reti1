//20015308 Umberto Uslenghi 26 agosto 2022

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>

#define DIM 256

void controlloInputRigaComando(int argc, char *argv[]);

int main(int argc, char *argv[])
{
  int simpleSocket = 0;
  int simplePort = 0;
  int returnStatus = 0;

  struct sockaddr_in simpleServer;

  controlloInputRigaComando(argc, argv);

  /* create a streaming socket */
  simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (simpleSocket == -1)
  {
    fprintf(stderr, "\nAttenzione:\ncreazione socket non riuscita!\n\n");
		exit(1);
  }
  else
	{
		fprintf(stdout, "\n\nSocket creata!\n\n");
  }

  /* retrieve the port number for connecting */
  //simplePort = atoi(argv[2]);
  simplePort = strtol(argv[2], NULL, 10);

  /* setup the address structure */
  /* use the IP address sent as an argument for the server address */
  //bzero(&simpleServer, sizeof(simpleServer));
  memset(&simpleServer, '\0', sizeof(simpleServer));
  simpleServer.sin_family = AF_INET;
  //inet_addr(argv[2], &simpleServer.sin_addr.s_addr);
  simpleServer.sin_addr.s_addr=inet_addr(argv[1]);
  simpleServer.sin_port = htons(simplePort);

  /* connect to the address and port with our socket */
  returnStatus = connect(simpleSocket, (struct sockaddr*)&simpleServer, sizeof(simpleServer));
  if (returnStatus == 0)
  {
    fprintf(stdout, "Connessione completata!\n\n\n");
  }
  else
  {
    fprintf(stderr, "Attenzione:\nconnessione non riuscita!\n\n");
		close(simpleSocket);
		exit(1);
  }

  char benvenutoServer[DIM] = "";
  strcpy(benvenutoServer, "");
  returnStatus = read(simpleSocket, benvenutoServer, sizeof(benvenutoServer));
	if(returnStatus <= 0)
	{
    fprintf(stderr, "\nAttenzione:\nerrore durante la lettura da parte del Client\n "
    "sulla socket dove il Server aveva scritto il messaggio di benvenuto.\n"
    "Chiusura della singola attuale connessione con il Client in corso.\n"
    "Il server rimane disponibile.\n\n");
    close(simpleSocket);
		exit(1);
	}

  char tempBuffer[DIM] = "";
  char *tempArrStr[DIM];
  char spazio[2]= " ";
  char invio[2]= "\n";
  char *tokenSpazio;
  char *savePtrStrTokR;
  int indexTempArrStr = 0;
  int tentativiServer;

  strcpy(tempBuffer, "");
  strcpy(tempBuffer, benvenutoServer);
  tokenSpazio = strtok_r(tempBuffer, spazio, &savePtrStrTokR);
  while(tokenSpazio != NULL)
  {
    tempArrStr[indexTempArrStr++] = tokenSpazio;
    tokenSpazio = strtok_r(NULL, spazio, &savePtrStrTokR);
  }

  if(strcmp(tempArrStr[0], "OK") == 0 && benvenutoServer[2] == 32
    && (isdigit(benvenutoServer[3]) != 0
        || (isdigit(benvenutoServer[3]) != 0 && isdigit(benvenutoServer[4]) != 0))
    && (benvenutoServer[4] == 32 || benvenutoServer[5] == 32)
    && benvenutoServer[strlen(benvenutoServer)-1] == 10)
  {
    tentativiServer = strtol(&benvenutoServer[3], NULL, 10);
    if (tentativiServer < 6 || tentativiServer > 10)
    {
      fprintf(stderr, "\nAttenzione:\nnumero non accettabile di tentativi inviati dal server.\n\n");
      close(simpleSocket);
  		exit(1);
    }

    if (isdigit(benvenutoServer[3]) != 0 && benvenutoServer[4] == 32)
    {
      for (int i = 5; benvenutoServer[i] != '\0'; i++)
      {
        fprintf(stdout, "%c", benvenutoServer[i]);
      }
      fprintf(stdout, invio);
    }
    else if (isdigit(benvenutoServer[3]) != 0 && isdigit(benvenutoServer[4]) != 0
                                                      && benvenutoServer[5] == 32)
    {
      for (int i = 6; benvenutoServer[i] != '\0'; i++)
      {
        fprintf(stdout, "%c", benvenutoServer[i]);
      }
      fprintf(stdout, invio);
    }
  }
  else
  {
    fprintf(stderr, "\nAttenzione:\nil messaggio di benvenuto è illegale ed illeggibile.\n\n");
    close(simpleSocket);
    exit(1);
  }

	while(1)
	{
    char clientBuffer[DIM] = "";
    char serverBuffer[DIM]= "";
    char tempBuffer2[DIM] = "";
    char *tempArrStr2[DIM];
    char spazio2[2]= " ";
    char *tokenSpazio2;
    char *savePtrStrTokR2;
    int indexTempArrStr2 = 0;
    int tentativoCorrente;
    int tentativiRimasti;

    strcpy(clientBuffer, "");
    strcpy(serverBuffer, "");

    fprintf(stdout, "I) effettuare un tentativo di identificazione digitando una parola di 5 caratteri e premendo 'invio'.\n"
    "II) terminare la comunicazione tramite il comando \"QUIT\" in maiuscolo e premere 'invio'.\n");

    fgets(clientBuffer, sizeof(clientBuffer), stdin);

    if (strlen(clientBuffer) == 6)
    {
      strcpy(serverBuffer, "WORD ");
      strcat(serverBuffer, clientBuffer);
      returnStatus= write(simpleSocket, serverBuffer, sizeof(serverBuffer));
      if(returnStatus <= 0)
      {
        fprintf(stderr, "\nAttenzione:\nerrore durante la scrittura da parte del Client\n "
        "sulla socket dove il Server avrebbe dovuto leggere il messaggio successivo.\n"
        "Chiusura della singola attuale connessione con il Client in corso.\n"
        "Il server rimane disponibile.\n\n");
        close(simpleSocket);
        exit(1);
      }
    }
    else
    {
      returnStatus= write(simpleSocket, clientBuffer, sizeof(clientBuffer));
      if(returnStatus <= 0)
      {
        fprintf(stderr, "\nAttenzione:\nerrore durante la scrittura da parte del Client\n "
        "sulla socket dove il Server avrebbe dovuto leggere il messaggio successivo.\n"
        "Chiusura della singola attuale connessione con il Client in corso.\n"
        "Il server rimane disponibile.\n\n");
        close(simpleSocket);
        exit(1);
      }
    }

    strcpy(clientBuffer, "");
    strcpy(tempBuffer2, "");

    returnStatus= read(simpleSocket, clientBuffer, sizeof(clientBuffer));
    if(returnStatus <= 0)
    {
      fprintf(stderr, "\nAttenzione:\nerrore durante la lettura da parte del Client\n "
      "sulla socket dove il Server aveva scritto il messaggio successivo.\n"
      "Chiusura della singola attuale connessione con il Client in corso.\n"
      "Il server rimane disponibile.\n\n");
      close(simpleSocket);
      exit(1);
    }

    strcpy(tempBuffer2, clientBuffer);

    tokenSpazio2 = strtok_r(tempBuffer2, spazio2, &savePtrStrTokR2);
    while(tokenSpazio2 != NULL)
    {
      tempArrStr2[indexTempArrStr2++] = tokenSpazio2;
      tokenSpazio2 = strtok_r(NULL, spazio2, &savePtrStrTokR2);
    }

    if(strcmp(tempArrStr2[0], "OK") == 0 && clientBuffer[2] == 32 && strcmp(tempArrStr2[1], "PERFECT\n") == 0
                                      && clientBuffer[10] == 10 && strlen(clientBuffer) == 11)
    {
      fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////////////////////\n");
      fprintf(stdout, "\n\t\tVITTORIA! Complimenti, hai indovinato la parola del giorno!\n\n");
      fprintf(stdout, "/////////////////////////////////////////////////////////////////////////////////////////\n\n");
      close(simpleSocket);
      return 0;
    }
    else if(strcmp(tempArrStr2[0], "OK") == 0 && clientBuffer[2] == 32
        && isdigit(clientBuffer[3]) != 0 && clientBuffer[4] == 32
        && (clientBuffer[5] == 42 || clientBuffer[5] == 43 || clientBuffer[5] == 45)
        && (clientBuffer[6] == 42 || clientBuffer[6] == 43 || clientBuffer[6] == 45)
        && (clientBuffer[7] == 42 || clientBuffer[7] == 43 || clientBuffer[7] == 45)
        && (clientBuffer[8] == 42 || clientBuffer[8] == 43 || clientBuffer[8] == 45)
        && (clientBuffer[9] == 42 || clientBuffer[9] == 43 || clientBuffer[9] == 45)
        && clientBuffer[10] == 10 && strlen(clientBuffer) == 11)
    {
      tentativoCorrente = strtol(&clientBuffer[3], NULL, 10);
      if (tentativoCorrente < 1 || tentativoCorrente > 10)
      {
        fprintf(stderr, "\nAttenzione:\nnumero di tentativo corrente non accettabile.\n\n");
        close(simpleSocket);
        exit(1);
      }

      fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////////////////////\n");
      fprintf(stdout, "\t\t\tTentativo %s / %d : %s\n", tempArrStr2[1], tentativiServer, tempArrStr2[2]);
      fprintf(stdout, "*\tindica che il carattere è corretto e si trova "
      "anche nella posizione \n\tcorretta tra i caratteri della parola da indovinare;"
      "\n+\tindica che il carattere è corretto, ma non si trova anche nella \n\t"
      "posizione corretta tra i caratteri della parola da indovinare;\n-\tindica "
      "che il carattere non è corretto e quindi non si trova tra \n\ti caratteri della parola da indovinare;\n\n");
      fprintf(stdout, "/////////////////////////////////////////////////////////////////////////////////////////\n\n");
    }
    else if(strcmp(tempArrStr2[0], "END") == 0 && clientBuffer[3] == 32
      && (isdigit(clientBuffer[4]) != 0
          || (isdigit(clientBuffer[4]) != 0 && isdigit(clientBuffer[5]) != 0))
      && (clientBuffer[5] == 32 || clientBuffer[6] == 32)
      && (clientBuffer[11] == 10 || clientBuffer[12] == 10)
      && (strlen(clientBuffer) == 12 || strlen(clientBuffer) == 13))
    {
      int caratteriClasseAlpha = 0;
      if (isdigit(clientBuffer[4]) != 0 && clientBuffer[5] == 32)
      {
        tentativiRimasti = strtol(&clientBuffer[3], NULL, 10);
        if (tentativiRimasti < 1 || tentativiRimasti > 10)
        {
          fprintf(stderr, "\nAttenzione:\n"
          "protocollo END violato dal messaggio del server: tentativi rimanenti non accettabili.\n\n");
          close(simpleSocket);
          exit(1);
        }

        for (int i = 0; tempArrStr2[2][i] != '\0'; i++)
        {
          if (isalpha(tempArrStr2[2][i]) != 0)
          {
              caratteriClasseAlpha++;
          }
        }

        if (caratteriClasseAlpha != 5)
        {
          fprintf(stderr, "\nAttenzione:\n"
          "protocollo END violato dal messaggio del server: parola target non alfabetica.\n\n");
          close(simpleSocket);
          exit(1);
        }

        fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(stdout, "\nTentativi terminati.\nHai avuto ben %s tentativi per indovinare la parola del giorno "
        "che era: %c%c%c%c%c.\nDavvero un peccato, ma puoi sempre eseguire nuovamente il programma client \ndell'applicazione di rete "
        "per provare ad essere più fortunato/a!\n\n", tempArrStr2[1], tempArrStr2[2][0], tempArrStr2[2][1], tempArrStr2[2][2], tempArrStr2[2][3], tempArrStr2[2][4]);
        fprintf(stdout, "/////////////////////////////////////////////////////////////////////////////////////////\n\n");
        close(simpleSocket);
        return 0;
      }
      else if (isdigit(clientBuffer[4]) != 0 && isdigit(clientBuffer[5]) != 0
                                                        && clientBuffer[6] == 32)
      {
        tentativiRimasti = strtol(&clientBuffer[3], NULL, 10);
        if (tentativiRimasti < 1 || tentativiRimasti > 10)
        {
          fprintf(stderr, "\nAttenzione:\n"
          "protocollo END violato dal messaggio del server: tentativi rimanenti non accettabili.\n\n");
          close(simpleSocket);
          exit(1);
        }

        for (int i = 0; tempArrStr2[2][i] != '\0'; i++)
        {
          if (isalpha(tempArrStr2[2][i]) != 0)
          {
              caratteriClasseAlpha++;
          }
        }
        if (caratteriClasseAlpha != 5)
        {
          fprintf(stderr, "\nAttenzione:\n"
          "protocollo END violato dal messaggio del server: parola target non alfabetica.\n\n");
          close(simpleSocket);
          exit(1);
        }

        fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////////////////////\n");
        fprintf(stdout, "\nTentativi terminati.\nHai avuto ben %s tentativi per indovinare la parola del giorno "
        "che era: %c%c%c%c%c.\nDavvero un peccato, ma puoi sempre eseguire nuovamente il programma client \ndell'applicazione di rete "
        "per provare ad essere più fortunato/a! Arrivederci!\n\n", tempArrStr2[1], tempArrStr2[2][0], tempArrStr2[2][1], tempArrStr2[2][2], tempArrStr2[2][3], tempArrStr2[2][4]);
        fprintf(stdout, "/////////////////////////////////////////////////////////////////////////////////////////\n\n");
        close(simpleSocket);
        return 0;
      }
    }
    else if(strcmp(tempArrStr2[0], "ERR") == 0 && clientBuffer[3] == 32
                          && clientBuffer[strlen(clientBuffer)-1] == 10)
    {
      fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////////////////////\n");
      for (int i = 4; clientBuffer[i] != '\0'; i++)
      {
        fprintf(stdout, "%c", clientBuffer[i]);
      }
      fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////////////////////\n\n");
      close(simpleSocket);
      exit(1);
    }
    else if(strcmp(tempArrStr2[0], "QUIT") == 0 && clientBuffer[4] == 32
                          && clientBuffer[strlen(clientBuffer)-1] == 10)
    {
      fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////////////////////\n\n\t\t\t");
      for (int i = 5; clientBuffer[i] != '\0'; i++)
      {
        fprintf(stdout, "%c", clientBuffer[i]);
      }
      fprintf(stdout, "\n/////////////////////////////////////////////////////////////////////////////////////////\n\n");
      close(simpleSocket);
      return 0;
    }
    else
    {
      fprintf(stderr, "\nAttenzione:\nmessaggio illegale rifiutato per la comunicazione dell'applicazione di rete.\n");
      close(simpleSocket);
      exit(1);
    }
	}

  close(simpleSocket);
  return 0;

}

void controlloInputRigaComando(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "\nModo d'uso dell'applicazione di rete: %s <indirizzo_server> <numero_porta>.\n\n", argv[0]);
		exit(1);
	}
	else
	{
    int controllo1_argv2 = 1;
    int controllo2_argv2 = 1;

    for (int i = 0; argv[1][i] != '\0'; i++)
		{
			if (isdigit(argv[1][i]) == 0 && argv[1][i] != 46)
			{
				controllo1_argv2 = 0;
				break;
			}
		}

    for (int i = 0; argv[2][i] != '\0'; i++)
    {
      if (isdigit(argv[2][i]) == 0)
      {
        controllo2_argv2 = 0;
        break;
      }
    }

		if (controllo1_argv2 == 0 || controllo2_argv2 == 0)
		{
			fprintf(stderr, "\nAttenzione:\ninserire <indirizzo_server> e <numero_porta> "
      "correttamente,\novvero l'indirizzo IPv4 del server da contattare e la porta sulla quale contattarlo.\n\n");
			exit(1);
		}
	}
}
