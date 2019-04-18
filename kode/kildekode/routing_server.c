#include <stdio.h>


int PORT;
int ;

int main(int argc, char* argv[]){

	printf("Hello I'm routing_server.c %d",argc);

    int i;

    printf("All Arguments.");
	for(i = 0; i < argc ; i ++){

		printf("Args : %d \n", argv[i]);

		if(argc == 1)
			PORT = atoi(argv[i]);
	}


	// Les data.txt.

	  // Lagre nodene i en datastruktur.

	// Utfør Dijsktras-algoritme for alle nodene og lagre det.





	// Åpne en Socket som lytter på en bestemt Port.





	return 0;
}