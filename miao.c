#include <stdio.h>      
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

int main(int argc, char const *argv[])
{
	char miao[512] = "OK START 10 11.6 24.7";
	char *ptr;
	int numeri = 0;
	numeri = strtol(miao, &ptr, 10);
	if (ptr == miao)
	{
		printf("ciao\n");
	}
}