#include <stdio.h> // For å ha access til printf()

#include <unistd.h> // For å kunne stenge en socket med close().
#include <sys/socket.h> // For det standardisérte socket-API'et.





int PORT = 0;
int N = 0;  // N = antall noder i systemet.

int main(int argc, char* argv[]){

	printf("Hello I'm routing_server.c %d",argc);

    int i;

    printf("All Arguments.\n");
	for(i = 0; i < argc ; i ++){

		printf("argv[%d] : %d \n", i , argv[i]);

		if(i == 1)
			PORT = atoi(argv[i]);

		if(i == 2){
			N = atoi(argv[i]);
		}
	}


	// Les data.txt.

    // Lagre nodene i en datastruktur.




    // Åpne en Socket som lytter på en bestemt Port
    // Og vent til Alle de N node-programmene har koblet seg til.

    // Så må jeg bygge opp en graf og kalkulere dijkstras-algoritme på den.
	





	return 0;
}