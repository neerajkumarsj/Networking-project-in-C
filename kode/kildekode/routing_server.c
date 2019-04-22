#include <stdio.h> // For å ha access til printf()

#include <unistd.h> // For å kunne stenge en socket med close().
#include <sys/socket.h> // For det standardisérte socket-API'et.
#include <sys/types.h> // For noen av typene som kreves for å få sockets til å funke.

#include <netinet/in.h>


int PORT = 0;
int N = 0;  // N = antall noder i systemet.

char clientResponseBuffer [2048];

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
	

	initializeTCPServer();




	return 0;
}


int nodeConnectionsCountSoFar = 0;


int initializeTCPServer(){
								// SOCK_STREAM betyr TCP-type socket.
	int ruterServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAdresse;
	
	serverAdresse.sin_family = AF_INET;
	serverAdresse.sin_port = htons(PORT);
	serverAdresse.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 ( localhost ).


	// Bind andressen ( dvs start socketen på en måte )
    bind(ruterServerSocket, (struct sockaddr *) &serverAdresse, sizeof(serverAdresse) );
    printf("bind() called.\n");

    // Og start å lytte på innkommende connections - N er max antall lovlige connections.
    listen(ruterServerSocket, N);

    printf("listen() called.\n");


    
    while(nodeConnectionsCountSoFar < N){


    	int klient_socket = accept(ruterServerSocket, NULL, NULL);
    	printf("accept() called.\n");

    	// recv(ruterServerSocket, char buffer[248], sizeof(buffer), 0);
    	recv(ruterServerSocket, &clientResponseBuffer, sizeof(clientResponseBuffer),0);
        printf("recieved message from socket %d   message:%s\n", klient_socket, clientResponseBuffer);


    }





    // if(socketStatus != -1){
    // 	// Alt gikk bra
    // }else{

    // 	// Error!
    // 	printf("Det skjedde en feil ved opprettelsen av TCP-router-serveren!");
    // 	exit(0);
    // 	return -1;

    // }

}
