#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For å kunne bruke memcpy.

#include <unistd.h> // For å kunne stenge en socket med close(). OG sleep(1) 
#include <sys/socket.h> // For det standardisérte socket-API'et.
#include <sys/types.h> // For noen av typene som kreves for å få sockets til å funke.

#include <netinet/in.h> // Inneholder noen av addresse structene som man bruker med sockets.


#include "IN2140Networking.h" // Inneholder structer for Oppgaven.


char buffer [2048];
int OwnAddress = -1;
int BasePort = -1;

char TCPBuffer [2048];


// Noen prototyper.
void initializeNode(int, char **);
void printRoutingTable(void);
int isDestinationInRoutingTable(int id);
int getNextHopByDestination(int dest);
void fetchRoutingTableFromServer(int serverSocket);
void listenForUDPPackages(void);
int openUDPConnectionsToNeighbours(void);



int neighbourCount = 0; // Amount of neighbouring nodes.
struct NodeInfo * neighbourNodes; // Array that holds references to all neighboring nodes and their weights.

struct RoutingTableNode ** routingTable; // Inneholder informasjon om alle nestehoppene denne noden er ansvarlig for.
int tableSize = 0;


int UDPserverSocket = -1;
struct sockaddr_in udpServerAdresse;

int main(int argc, char * argv[]){

	printf("Hello I'm a node.c! Amount of arguments: %d \n",argc);
	
	initializeNode(argc, argv);

	openTCPConnectionToRoutingServer();

    // Print some statistics to make sure it was done right.
    printNodeInfo();

	UDPserverSocket = openUDPServerConnection();

    if(UDPserverSocket != -1){
        printf("UDP ServerSocket was successfully created! Port: %d \n", BasePort+OwnAddress);
    }



    int result = openUDPConnectionsToNeighbours();

    listenForUDPPackages();
	

    freeAllAllocatedMemory();

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



        /// Clear the TCPBuffer first. It might have dirty data on it.
        printf("\n1024 first bytes of TCPBuffer BEFORE network-preparation =  \n");
        for(i = 0; i< 2048; i++){
            TCPBuffer[i] = 0;
        }

    	int lastIndex = 0;
    	memcpy(&TCPBuffer, &OwnAddress, sizeof(int));
    	lastIndex = sizeof(int);

    	//snprintf(TCPBuffer, sizeof(buffer), "%d\n%s", OwnAddress, edgeWeightList);
    	memcpy(&TCPBuffer[lastIndex], &edgeWeightList, curIndex);



    	printf("\n2048 first bytes of TCPBuffer after network-preparation =  \n");
    	for(i = 0; i< 2048; i++){
    		printf("%d",TCPBuffer[i]);
    	}



    	// #2 Send informasjonen for denne Noden.

    	// eksempel
    	// send(socketToRouter, char buffer[248], sizeof(buffer), 0) 
    	// printf("Attempting to send data: %s\n", TCPBuffer);
        int bytesSent = send(socketToRouter,TCPBuffer, sizeof(TCPBuffer),0);

        printf("returnvalue from send() : %d\n", bytesSent);


        for(i = 0; i < 2048 ; i++){
            TCPBuffer[i] = 0;
        }


        
        fetchRoutingTableFromServer(socketToRouter);

        printRoutingTable();


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


void fetchRoutingTableFromServer(int serverSocket){


        // #3 Vent på ruter-data sendt med TCP fra hovedprogrammet.
        int bytesRecieved = recv(serverSocket, &TCPBuffer, sizeof(TCPBuffer),0 );

        printf("returnvalue from recv() : %d\n", bytesRecieved);
        int i; 
        for(i = 0; i < 2048 ; i++){
            printf("%d",TCPBuffer[i]);
        }

        int readIndex = 0; 
      
        // #4 Lagre rute-tabellen

        // Først størrelsen som lagres i Table size
        memcpy(&tableSize, &TCPBuffer, sizeof(int));
        tableSize = tableSize / 2 / 4; 


        printf("Amount of nextHops stored in the routing table: %d ", tableSize);
        routingTable = malloc(sizeof(struct RoutingTableNode * ) * tableSize);

        
        readIndex = sizeof(int);

        for(i = 0; i < tableSize ; i++){

            // allokér minne til en struct RoutingTableNode
            routingTable[i] = (struct RoutingTableNode * ) malloc(sizeof(struct RoutingTableNode));

            // Kopiér inn en int som henviser til destinasjon
            memcpy(&routingTable[i]->destination, &TCPBuffer[readIndex], sizeof(int));
            readIndex += sizeof(int);

            // Kopiér inn en int som henviser til neste Node hvis man skal til destinasjonen.
            memcpy(&routingTable[i]->nextHop, &TCPBuffer[readIndex], sizeof(int));
            readIndex += sizeof(int);            
        }



}


int openUDPServerConnection(){

    int socketToRouter = socket(AF_INET, SOCK_DGRAM, 0);

    if(socketToRouter == -1 ){
        printf("\nCreating the socket failed!\n");
        exit(EXIT_FAILURE);
    }

    udpServerAdresse.sin_family = AF_INET;
    udpServerAdresse.sin_port = htons(BasePort+OwnAddress);
    udpServerAdresse.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 ( localhost ).

    if ( bind(socketToRouter, (struct sockaddr *) &udpServerAdresse, sizeof(udpServerAdresse)) < 0 ){ 
        printf("\nBinding the socket failed!!! :/ Try another port number! :)  \n");
        exit(EXIT_FAILURE); 
    } 

    return socketToRouter;
}


int openUDPConnectionsToNeighbours(){

    // Test::  prøv å connect til Node 11 fra 1.
    if(OwnAddress == 1){

        struct sockaddr_in udpClientAddress;

        int socket11 = socket(AF_INET, SOCK_DGRAM, 0);
        if(socket11 < 0){
            printf("UDP-Connection from  %d -> %d failed!\n", OwnAddress, 11);
            exit(EXIT_FAILURE);
        }

        // Filling server information 
        udpClientAddress.sin_family = AF_INET; 
        udpClientAddress.sin_port = htons(BasePort + 11); 
        udpClientAddress.sin_addr.s_addr = INADDR_ANY;

        char * str = "omggg\0";
        sendto(socket11, str , str, MSG_CONFIRM, (struct udpClientAddress *) &udpClientAddress,sizeof(udpClientAddress));


    }


    return -1;
}

void parseCommandFromUDPSocket(){

}


void listenForUDPPackages(){
    // n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
    //             MSG_WAITALL, (struct sockaddr *) &servaddr, 
    //             &len); 

    char UDPBuffer [2048]; 
    int bytesRecieved = recvfrom(UDPserverSocket, UDPBuffer
         , 2048 , MSG_WAITALL, (struct sockaddr_in *) &udpServerAdresse, sizeof(udpServerAdresse));

    printf("\nUDPBuffer value: \n", UDPBuffer);
}


int UPDpreparePackage(int nodeIDTo){


}

int isDestinationInRoutingTable(int id){
    int i; 
    printf("\n\n");
    for( i = 0 ; i < tableSize; i++){
        if(id == routingTable[i]->destination){
            return 1;
        }
    }

    return -1;
}

int getNextHopByDestination(int dest){
    int i; 
    printf("\n\n");
    for( i = 0 ; i < tableSize; i++){
        if(dest == routingTable[i]->destination){
            return routingTable[i]->nextHop;
        }
    }
    return -1;
}

void printRoutingTable(){
    int i; 
    printf("\n\n");
    for( i = 0 ; i < tableSize; i++){
        printf("routingTable[%d] destination:%d -- nextHop -> %d \n", i , routingTable[i]->destination, routingTable[i]->nextHop);
    }
}


void freeAllAllocatedMemory (){
    int i;
    for(i = 0; i < tableSize ; i++){
        // Frigjør pekerene til RoutingTableNode
        //free(routingTable[i]);
    }
    // Frigjør hele Routing Tabelet
    free(routingTable);
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







