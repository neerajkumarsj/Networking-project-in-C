#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For å kunne bruke memcpy.

#include <unistd.h> // For å kunne stenge en socket med close().
#include <sys/socket.h> // For det standardisérte socket-API'et.
#include <sys/types.h> // For noen av typene som kreves for å få sockets til å funke.

#include <netinet/in.h> // Inneholder noen av addresse structene som man bruker med sockets.


#include "IN2140Networking.h" // Inneholder structer for Oppgaven.


char buffer [2048];
int OwnAddress = -1;
int BasePort = -1;

void initializeNode(int, char **);

int neighbourCount = 0; // Amount of neighbouring nodes.
struct NodeInfo * neighbourNodes; // Array that holds references to all neighboring nodes and their weights.




int main(int argc, char * argv[]){

	printf("Hello I'm a node.c! Amount of arguments: %d \n",argc);
	
	initializeNode(argc, argv);

	openTCPConnectionToRoutingServer();

	openUDPConnectionsToNeighbours();

	printNodeInfo();

	return 0;
}









void initializeNode(int argCount, char * argList[]){
	
	printf("Initializing node.\n");
    // If the arguments to this program are formatted correctly
    // argc - 3 should be the correct size of the Edge -> Weight Array.
    neighbourCount = argCount - 3;
    neighbourNodes = malloc(sizeof(struct NodeInfo) * neighbourCount);
    int currentNeighbourIndex = 0;
    int i;

    printf("Reading arguments:\n");
	for(i = 0; i < argCount ; i++){
		printf("argument %d: %s \n",i, argList[i]);


		if( i == 1 ){
			BasePort = atoi(argList[i]);
		}

		if( i == 2 ){
			OwnAddress = atoi(argList[i]);
		}


		// Lagre Neighbooring Nodes + Weights.
		if( i >= 3){
            
			// Så må vi allokere minne til structen som skal lagres.
			neighbourNodes[currentNeighbourIndex].from = OwnAddress;

			// Leser inn informasjon i structen fra argumentet.
			sscanf(argList[i], "%d:%d", &neighbourNodes[currentNeighbourIndex].to, &neighbourNodes[currentNeighbourIndex].weight);

            currentNeighbourIndex++;

		}

	}
}




int openTCPConnectionToRoutingServer(){
 								// SOCK_STREAM betyr TCP-type socket.
	int socketToRouter = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serverAdresse;
	
	serverAdresse.sin_family = AF_INET;
	serverAdresse.sin_port = htons(BasePort);
	serverAdresse.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 ( localhost ).

    int socketStatus = connect(socketToRouter, (struct sockaddr *) &serverAdresse, sizeof(serverAdresse) );

    if(socketStatus != -1){
    	// Alt gikk bra og dermed kan vi sende data direkte!

    	// #1 Forberéd dataen som skal sendes.

    	// Protokoll:
    	// OwnAdress |    Edge      Weight    |          |     Edge      Weight    |     '\0'
    	// 4 bytes   |    4 bytes   4 bytes   |     ...  |     4 bytes   4 bytes   |     1 byte.

    	char edgeWeightList [2048];
        int i; 
        printf("\n1024 first bytes of edgeWeightList BEFORE network-preparation =  \n");
        for(i = 0; i< 2048; i++){
            edgeWeightList[i] = 0;
        }


    	int curIndex = 0;
          printf("Looping through neighbours: \n ");
    	  for(i = 0; i < neighbourCount; i++){

    	  	printf("Dealing with neighbour[%d]  to= %d     weight= %d \n" , i, neighbourNodes[i].to, neighbourNodes[i].weight);

                edgeWeightList[curIndex] = neighbourNodes[i].to;
                curIndex += sizeof(int);
                edgeWeightList[curIndex] = neighbourNodes[i].weight;
                curIndex += sizeof(int);
                

            printf("Index after adding weight: curIndex = %d\n", curIndex);

    	 }

    	// Set nullchar at the end of the edgeWeightList for this Node.
    	edgeWeightList [curIndex] = '\0';
    	printf("\n");
    	printf("256 first bytes of edgeWeightList after network-preparation =  ");

    	for(i = 0; i< 256; i++){
    		printf("%d",edgeWeightList[i]);
    	}


        printf("\n");
    	printf("Connection with RoutingServer successfull! \n ");



    	char nodeInfoTCPBuffer [2048];


        /// Clear the nodeInfoTCPBuffer first. It might have dirty data on it.
        printf("\n1024 first bytes of nodeInfoTCPBuffer BEFORE network-preparation =  \n");
        for(i = 0; i< 2048; i++){
            nodeInfoTCPBuffer[i] = 0;
        }

    	int lastIndex = 0;
    	memcpy(&nodeInfoTCPBuffer, &OwnAddress, sizeof(int));
    	lastIndex = sizeof(int);

    	//snprintf(nodeInfoTCPBuffer, sizeof(buffer), "%d\n%s", OwnAddress, edgeWeightList);
    	memcpy(&nodeInfoTCPBuffer[lastIndex], &edgeWeightList, curIndex);



    	printf("\n1024 first bytes of nodeInfoTCPBuffer after network-preparation =  \n");
    	for(i = 0; i< 1024; i++){
    		printf("%d",nodeInfoTCPBuffer[i]);
    	}



    	// #2 Send informasjonen for denne Noden.

    	// eksempel
    	// send(socketToRouter, char buffer[248], sizeof(buffer), 0) 
    	// printf("Attempting to send data: %s\n", nodeInfoTCPBuffer);
        int bytesSent = send(socketToRouter,nodeInfoTCPBuffer, sizeof(nodeInfoTCPBuffer),0);

        printf("returnvalue from send() : %d", bytesSent);







        // #3 Vent på ruter-data fra hovedprogrammet




        // #4 Lagre rute-tabellen 





        // #5 Terminér TCP-tilkoblingen med rute_serveren.
        close(socketToRouter);

    }else{

    	// Error!
    	printf("Det skjedde en feil ved opprettelsen av TCP-router-klient-socketen!\n");
    	exit(0);
    	return -1;

    }

	return 0;
}



int openUDPConnectionsToNeighbours(){

	return 0;
}


void parseCommandFromUDPSocket(){

}




void freeAllAllocatedMemory (){

	free(neighbourNodes);
}



int printNodeInfo(){
	printf("Node Information:::::\n----------------\n");
	printf("OwnAddress: %d \n", OwnAddress);
	printf("BasePort  : %d \n", BasePort);
	printf("\n");
	int i = 0; 
	for(; i < neighbourCount ; i++){
	    printf("frm: %d  to:   %d   weight: %d \n", neighbourNodes[i].from, neighbourNodes[i].to, neighbourNodes[i].weight);
	}
}







