#include <stdio.h> // For å ha access til printf()
#include <stdlib.h> // For å kunne allokére med malloc()
#include <string.h> // For å kunne bruke memcpy.

#include <unistd.h> // For å kunne stenge en socket med close().
#include <sys/socket.h> // For det standardisérte socket-API'et.
#include <sys/types.h> // For noen av typene som kreves for å få sockets til å funke.

#include <netinet/in.h>

#include "IN2140Networking.h" // Inneholder structer for Oppgaven.


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


    // Lag en NodeSocket liste som alle nodene kan lagres i. ( Vil brukes som Graf for Dijsktras senere ).
    
    struct NodeSocket ** nodeSockets = malloc(sizeof(struct NodeSocket *) * N);
    int currentNodeSocketCount = 0;


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

        printf("N = %d,   nodeConnectionsCountSoFar = %d\n",N, nodeConnectionsCountSoFar);

    	int klient_socket = accept(ruterServerSocket, NULL, NULL);
    	printf("accept() called!\n");

        if(klient_socket == -1){

            printf("Accept() called but client_socket has a failure value.\n");

        }else{
            printf("A TCP-client connected!!\n client_socket_id = %d \n", klient_socket);

            // Handle Storage of the client structre here:
            nodeSockets[currentNodeSocketCount] = malloc(sizeof(struct NodeSocket) );
            nodeSockets[currentNodeSocketCount]->socketID = klient_socket;
            nodeSockets[currentNodeSocketCount]->noder = NULL;
            currentNodeSocketCount ++;

            int bytesRecieved = recv(klient_socket, &clientResponseBuffer, sizeof(clientResponseBuffer),0);

            if(bytesRecieved > 0){

                printf("%d bytes Recieved!! : Recieved message from socket: %d   message:%s\n", bytesRecieved, klient_socket, clientResponseBuffer);


                // Decode data from socket.
                struct NodeSocket * currSock = getNodeSocketBySocketId(nodeSockets, currentNodeSocketCount, klient_socket);

                // Copy in the ID of the node.
                memcpy(&currSock->nodeID, &clientResponseBuffer, sizeof(int));
                int readIndex = sizeof(int);



                // Copy in all the weights.
                while(clientResponseBuffer[readIndex] != '\0'){
                    int to     =  clientResponseBuffer[readIndex];
                    readIndex += 4; 
                    int weight =  clientResponseBuffer[readIndex];
                    readIndex += 4;
                    printf("nodeID[%d] has Edge/weight;  to:  %d     weight:    %d \n",currSock->nodeID, to,weight );
                }


                // amount of weights = 



            } else if(bytesRecieved == 0){

                printf("No bytes were recieved, but a sending-connection was made from a client.\n");

            } else{

                printf("An error occured with reception of data!\n");   
            }

                
                




            if(currentNodeSocketCount > N){
                printf("Amount of nodes connected in the router exceeded the amount specified at start of Program: Max Sockets is: %d \n",N);
                exit(0);
            }
            // 1) Store Socket ID.
            // 2) Store NodeID.
            // 3) Store List over Edge - Weights.

            printf("All nodes currently in system: \n");
            printAllNodeSockets(nodeSockets, currentNodeSocketCount);

        }




        // RECEPTION OF DATA.



    	// recv(ruterServerSocket, char buffer[248], sizeof(buffer), 0);
    	
        


        // Store the Socket connection and the data from the Socket.



        // Increase the amonut of stored Data.
        nodeConnectionsCountSoFar++;


    }

    // When the code reaches this point all the nodes have successfully communicated
    // with the routing_server.





    // if(socketStatus != -1){
    // 	// Alt gikk bra
    // }else{

    // 	// Error!
    // 	printf("Det skjedde en feil ved opprettelsen av TCP-router-serveren!");
    // 	exit(0);
    // 	return -1;

    // }


    // Free up dynamically allocated memory from nodeSockets.
    free(nodeSockets);
    int i;
    for(i = 0; i < currentNodeSocketCount ; i++){
        free(nodeSockets[i]);
    }

}


void printAllNodeSockets(struct NodeSocket * sockets [], int len ){

    int i = 0;
    for (; i < len ; i++){
        printf("sockets[%d]->socketID --> %d ", i,  sockets[i]->socketID);
        printf("sockets[%d]->nodeID --> %d ", i,  sockets[i]->nodeID);
        printf("sockets[%d]->noder ---> %d ", i, sockets[i]->noder); 
        printf("\n");
    
    }  

}



struct NodeSocket * getNodeSocketBySocketId(struct NodeSocket * sockets [], int len,  int socketID){
    int i;
    for(i = 0; i < len; i++  ){
        if(sockets[i]->socketID == socketID){
            return sockets[i];
        }
    }

}













