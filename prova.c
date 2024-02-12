#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

int main(int argc, char const *argv[])
{
	int indice;
	long valori[128];
	char buffer[512];
	long numero;
	char *ptr;
	char *buff;
	float media = 0;
	float varianza = 0;
	fgets(buffer, sizeof(buffer), stdin);
	buff = buffer;
	printf("%s\n", buffer);
	numero = strtol(buff, &ptr, 10);
	buff = ptr;
	//printf("%s\n", token);
	//indice = atoi(token);
	indice = numero;
	printf("Indice:%d\n", indice);

	for (int i = 0; *buff != '\n'; ++i){
		numero = strtol(buff, &ptr, 10);
		valori[i] = numero;
		if (ptr == buff)
		{
			printf("No\n");
			exit(0);
		}
		buff = ptr;
	}

	for (int i = 0; i < indice; ++i){
		printf("%ld ",valori[i]);
	}
	printf("\n");

	for (int i = 0; i < indice; ++i){
		media = media + valori[i];
	}
	media = media/indice;

	for (int i = 0; i < indice; ++i){
		varianza = varianza + (valori[i]-media)*(valori[i]-media);
	}
	varianza = varianza/indice;

	printf("Media:%f\nVarianza:%f\n",media, varianza);
}