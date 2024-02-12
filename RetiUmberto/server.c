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

void controlloInputRigaComando(int argc, char *argv[], int *tentativiInput);

void benvenuto(int argc, char *argv[], int *tentativiInput,
	int *simpleChildSocket, char messaggioBenvenutoDefault[],
	char messaggioBenvenuto[], char messaggioBenvenutoNonDefault1[],
	char messaggioBenvenutoNonDefault2[]);

int main(int argc, char *argv[])
{
	int simpleSocket = 0;
	int simplePort = 0;
	int returnStatus = 0;

	int tentativiInput;

	struct sockaddr_in simpleServer;

	controlloInputRigaComando(argc, argv, &tentativiInput);

	/* create a streaming socket */
	simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (simpleSocket == -1)
	{
		fprintf(stderr, "\nCreazione socket non riuscita!\n\n");
		exit(1);
	}
	else
	{
		fprintf(stdout, "\n\nSocket creata!\n\n");
  }

  /* retrieve the port number for listening */
  //simplePort = atoi(argv[1]);
	simplePort = strtol(argv[1], NULL, 10);

  /* setup the address structure */
  /* use INADDR_ANY to bind to all local addresses */
  memset(&simpleServer, '\0', sizeof(simpleServer));
  simpleServer.sin_family = AF_INET;
	simpleServer.sin_addr.s_addr = htonl(INADDR_ANY);
  simpleServer.sin_port = htons(simplePort);

  /* bind to the address and port with our socket */
  returnStatus = bind(simpleSocket,(struct sockaddr*)&simpleServer,sizeof(simpleServer));
	if (returnStatus != 0)
	{
		fprintf(stderr, "Connessione non riuscita!\n\n");
		close(simpleSocket);
		exit(1);
	}
	else
	{
		fprintf(stdout, "Connessione completata!\n\n\n");
	}

  /* lets listen on the socket for connections */
  returnStatus = listen(simpleSocket, 5);
  if (returnStatus == -1)
	{
		fprintf(stderr, "Socket non in ascolto!\n\n");
		close(simpleSocket);
		exit(1);
	}

	while (1)
	{
		struct sockaddr_in clientName = { 0 };
		int simpleChildSocket = 0;
		unsigned int clientNameLength = sizeof(clientName);

		/* wait here */
    simpleChildSocket = accept(simpleSocket,(struct sockaddr *)&clientName, &clientNameLength);
		if (simpleChildSocket == -1)
		{
			fprintf(stderr, "Non sono ammesse ulteriori connessioni!\n\n");
			close(simpleSocket);
			exit(1);
		}

		srand(time(NULL));
		int numeroParole = 10;
		char *paroleCasuali[DIM] = {"opera", "sosia", "ferro", "legna", "podio",
																"acuto", "abete", "sedia", "barca", "sorte"};
		char *target = paroleCasuali[rand() % numeroParole];

		int tentativiContatore;
		int tentativiContatoreDifferenza;
		tentativiContatore = tentativiInput;

		char messaggioBenvenuto[DIM] = "";
		char messaggioBenvenutoDefault[DIM] = "OK 6 Benvenuto/a! "
    "Prova ad indovinare la parola del giorno in 6 tentativi.\n"
    "I) Per fare un tentativo:\ndigita 5 caratteri alfabetici e dopo premi 'invio'.\n"
    "II) Per terminare la comunicazione:\ndigita in maiuscolo il comando \"QUIT\" e dopo premi 'invio'.\n";
		char messaggioBenvenutoNonDefault1[DIM] = " Benvenuto/a! Prova ad indovinare la parola del giorno in ";
		char messaggioBenvenutoNonDefault2[DIM] = " tentativi.\n"
	  "I) Per fare un tentativo:\ndigita 5 caratteri alfabetici e dopo premi 'invio'.\n"
	  "II) Per terminare la comunicazione:\ndigita in maiuscolo il comando \"QUIT\" e dopo premi 'invio'.\n";

		benvenuto(argc, argv, &tentativiInput,
			&simpleChildSocket, messaggioBenvenutoDefault,
			messaggioBenvenuto, messaggioBenvenutoNonDefault1, messaggioBenvenutoNonDefault2);

		while(1)
		{
			char serverBuffer[DIM] = "";
			char clientBuffer[DIM] = "";
			char tempBuffer[DIM] = "";
			char *tempArrStr[DIM];
			char spazio[2]= " ";
			char *tokenSpazio;
			char *savePtrStrTokR;
			int indexTempArrStr = 0;

			strcpy(serverBuffer, "");
			strcpy(clientBuffer, "");
			strcpy(tempBuffer, "");

			returnStatus = read(simpleChildSocket, clientBuffer, sizeof(clientBuffer));
			if(returnStatus <= 0)
			{
				fprintf(stderr, "\nAttenzione:\nerrore durante la lettura della socket "
				"da parte del Server\noppure connessione con il Client terminata prematuramente\ncon "
				"il comando 'ctrl+C' direttamente dal terminale e per\nquesto motivo il Client non ha scritto nulla "
				"sulla\nsocket da cui il Server avrebbe dovuto leggere il messaggio successivo.\n"
				"Chiusura della singola attuale connessione con il Client in corso.\n"
				"Il server rimane disponibile.\n\n");
				break;
			}

			for (int i = 0; clientBuffer[i]!= '\0'; i++)
			{
	      if (isascii(clientBuffer[i]) == 0)
				{
					strcpy(serverBuffer, "ERR \nAttenzione:\nè stato digitato un carattere non ASCII.\n"
					"Chiusura della connessione in corso.\n"
					"Il server rimane disponibile.\n");
					returnStatus= write(simpleChildSocket, serverBuffer, sizeof(serverBuffer));
					if(returnStatus <= 0)
					{
						fprintf(stderr, "\nAttenzione:\nerrore durante la scrittura da parte del Server\n "
						"sulla socket dove il Client avrebbe dovuto leggere il messaggio successivo.\n"
						"Chiusura della singola attuale connessione con il Client in corso.\n"
						"Il server rimane disponibile.\n\n");
						break;
					}
					break;
				}
			 }

			strcpy(tempBuffer, clientBuffer);
	    tokenSpazio = strtok_r(tempBuffer, spazio, &savePtrStrTokR);
	    while(tokenSpazio != NULL)
	    {
	      tempArrStr[indexTempArrStr++] = tokenSpazio;
	      tokenSpazio = strtok_r(NULL, spazio, &savePtrStrTokR);
	    }

			if(strcmp(tempArrStr[0], "WORD") == 0 && clientBuffer[4] == 32
					&& isalpha(clientBuffer[5]) != 0 && isalpha(clientBuffer[6]) != 0
					&& isalpha(clientBuffer[7]) != 0 && isalpha(clientBuffer[8]) != 0
					&& isalpha(clientBuffer[9]) != 0 && clientBuffer[10] == 10 && strlen(clientBuffer) == 11)
			{
				int contatoreCaratteriUguali = 0;
				char guess[DIM] = "";
				strcpy(guess, "");
				strcpy(guess, tempArrStr[1]);

				for(int i = 0; target[i] != '\0'; i++)
			  {
			    guess[i] = tolower(guess[i]);
			    if(guess[i] == target[i])
			    {
			      contatoreCaratteriUguali++;
			    }
			  }

				if(contatoreCaratteriUguali != 5)
				{
					char invio[2]= "\n";
					char tentativiChar[2] = "";

					tentativiContatore--;
					tentativiContatoreDifferenza = tentativiInput - tentativiContatore;

					sprintf(tentativiChar, "%d", tentativiContatoreDifferenza);

					if(tentativiContatore == 0)
					{
						strcpy(serverBuffer, "END ");
						strcat(serverBuffer, tentativiChar);
						strcat(serverBuffer, spazio);
						strcat(serverBuffer, target);
						strcat(serverBuffer, invio);

						returnStatus= write(simpleChildSocket, serverBuffer, sizeof(serverBuffer));
						if(returnStatus <= 0)
						{
							fprintf(stderr, "\nAttenzione:\nerrore durante la scrittura da parte del Server\n "
							"sulla socket dove il Client avrebbe dovuto leggere il messaggio successivo.\n"
							"Chiusura della singola attuale connessione con il Client in corso.\n"
							"Il server rimane disponibile.\n\n");
							break;
						}
						break;
					}
					else
					{
						strcpy(serverBuffer, "OK ");
						strcat(serverBuffer, tentativiChar);
						strcat(serverBuffer, spazio);

						for(int i = 0; target[i] != '\0'; i++)
						{
							if(target[i] == guess[i])
							{
								strcat(serverBuffer, "*");
							}
							else
							{
								int charPresente = 0;
								for(int j = 0; target[j] != '\0'; j++)
								{
									if(target[j] == guess[i])
									{
										strcat(serverBuffer, "+");
										charPresente = 1;
										break;
									}
								}
								if(charPresente == 0)
								{
									strcat(serverBuffer, "-");
								}
							}
						}
						strcat(serverBuffer, invio);

						returnStatus= write(simpleChildSocket, serverBuffer, sizeof(serverBuffer));
						if(returnStatus <= 0)
						{
							fprintf(stderr, "\nAttenzione:\nerrore durante la scrittura da parte del Server\n "
							"sulla socket dove il Client avrebbe dovuto leggere il messaggio successivo.\n"
							"Chiusura della singola attuale connessione con il Client in corso.\n"
							"Il server rimane disponibile.\n\n");
							break;
						}
					}
				}
				else
				{
					strcpy(serverBuffer, "OK PERFECT\n");
					returnStatus= write(simpleChildSocket, serverBuffer, sizeof(serverBuffer));
					if(returnStatus <= 0)
					{
						fprintf(stderr, "\nAttenzione:\nerrore durante la scrittura da parte del Server\n "
						"sulla socket dove il Client avrebbe dovuto leggere il messaggio successivo.\n"
						"Chiusura della singola attuale connessione con il Client in corso.\n"
						"Il server rimane disponibile.\n\n");
						break;
					}
					break;
				}
			}
			else if(strcmp(tempArrStr[0], "QUIT\n") == 0 && strlen(clientBuffer) == 5)
			{
				strcpy(serverBuffer, "QUIT Vai via cosi' presto? La parola era: ");
				strcat(serverBuffer, target);
				strcat(serverBuffer, ".\n");
				returnStatus= write(simpleChildSocket, serverBuffer, sizeof(serverBuffer));
				if(returnStatus <= 0)
				{
					fprintf(stderr, "\nAttenzione:\nerrore durante la scrittura da parte del Server\n "
					"sulla socket dove il Client avrebbe dovuto leggere il messaggio successivo.\n"
					"Chiusura della singola attuale connessione con il Client in corso.\n"
					"Il server rimane disponibile.\n\n");
					break;
				}
				break;
			}
			else
			{
					strcpy(serverBuffer, "ERR \nAttenzione:\ncomando non disponibile o non in maiuscolo.\nRicordarsi "
				  "di fare un tentativo esclusivamente con 5 caratteri alfabetici.\n"
				  "Non utilizzare accenti, spazi o tabulazioni.\n"
				  "Chiusura della connessione in corso.\n"
				  "Il server rimane disponibile.\n");
					returnStatus= write(simpleChildSocket, serverBuffer, sizeof(serverBuffer));
					if(returnStatus <= 0)
					{
						fprintf(stderr, "\nAttenzione:\nerrore durante la scrittura da parte del Server\n "
						"sulla socket dove il Client avrebbe dovuto leggere il messaggio successivo.\n"
						"Chiusura della singola attuale connessione con il Client in corso.\n"
						"Il server rimane disponibile.\n\n");
						break;
					}
					break;
			}
		}
		close(simpleChildSocket);
	}
    close(simpleSocket);
    return 0;
}

void controlloInputRigaComando(int argc, char *argv[], int *tentativiInput)
{
	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "\nModo d'uso dell'applicazione di rete:\n%s <numero porta> "
		"oppure %s <numero porta> [<max tentativi>].\n\n", argv[0], argv[0]);
		exit(1);
	}
	else
	{
		if (argc == 2)
		{
			int controllo1_argv2 = 1;
			for (int i = 0; argv[1][i] != '\0'; i++)
			{
				if (isdigit(argv[1][i]) == 0)
				{
					controllo1_argv2 = 0;
					break;
				}
			}
			if (controllo1_argv2 == 0)
			{
				fprintf(stderr, "\nAttenzione:\ninserire <numero_porta> "
				"correttamente,\novvero la porta sulla quale contattare l'indirizzo IPv4 del server.\n\n");
				exit(1);
			}
			else
			{
				*tentativiInput = 6;
			}
		}
		else if (argc == 3 && (strlen(argv[2])==1 || strlen(argv[2])==2))
		{
			int controllo2_argv2 = 1;
			int controllo3_argv2 = 1;
			for (int i = 0; argv[1][i] != '\0'; i++)
			{
				if (isdigit(argv[1][i]) == 0)
				{
					controllo2_argv2 = 0;
					break;
				}
			}
			for (int i = 0; argv[2][i] != '\0'; i++)
			{
				if (isdigit(argv[2][i]) == 0)
				{
					controllo3_argv2 = 0;
					break;
				}
			}
			if (controllo2_argv2 == 0 || controllo3_argv2 == 0)
			{
				fprintf(stderr, "\nAttenzione:\ninserire <numero_porta> "
				"correttamente,\novvero la porta sulla quale contattare l'indirizzo IPv4 "
				"del server e\nper l'input dei <tentativi> e' ammissibile unicamente un "
				"numero maggiore di 5 e minore di 11.\n\n");
				exit(1);
			}
			else
			{
				*tentativiInput = strtol(argv[2], NULL, 10);
				if (*tentativiInput < 6 || *tentativiInput > 10)
				{
					fprintf(stderr, "\nAttenzione:\nper l'input dei <tentativi> "
					"e' ammissibile unicamente un numero maggiore di 5 e minore di 11.\n\n");
					exit(1);
				}
			}
		}
	}
}

void benvenuto(int argc, char *argv[], int *tentativiInput,
	int *simpleChildSocket, char messaggioBenvenutoDefault[],
	char messaggioBenvenuto[], char messaggioBenvenutoNonDefault1[], char messaggioBenvenutoNonDefault2[])
{
	int returnStatus = 0;

	if (argc == 2 || *tentativiInput == 6)
	{
		returnStatus = write(*simpleChildSocket, messaggioBenvenutoDefault, strlen(messaggioBenvenutoDefault));
		if(returnStatus <= 0)
		{
			fprintf(stderr, "\nAttenzione:\nerrore durante l'invio del messaggio\n "
			"di benvenuto da parte del Server al Client.\n\n");
			return;
		}
		return;
	}
	else
	{
		strcpy(messaggioBenvenuto, "OK ");
		strcat(messaggioBenvenuto, argv[2]);
		strcat(messaggioBenvenuto, messaggioBenvenutoNonDefault1);
		strcat(messaggioBenvenuto, argv[2]);
		strcat(messaggioBenvenuto, messaggioBenvenutoNonDefault2);

		returnStatus = write(*simpleChildSocket, messaggioBenvenuto, strlen(messaggioBenvenuto));
		if(returnStatus <= 0)
		{
			fprintf(stderr, "\nAttenzione:\nerrore durante l'invio del messaggio\n "
			"di benvenuto da parte del Server al Client.\n\n");
			return;
		}
		return;
	}
}
