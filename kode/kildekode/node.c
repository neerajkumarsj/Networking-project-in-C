#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For å kunne bruke memcpy.

#include <unistd.h> // For å kunne stenge en socket med close(). OG sleep(1) 
#include <sys/socket.h> // For det standardisérte socket-API'et.
#include <sys/types.h> // For noen av typene som kreves for å få sockets til å funke.

#include <netinet/in.h> // Inneholder noen av addresse structene som man bruker med sockets.

#include "../print_lib/print_lib.h"
#include "IN2140Networking.h" // Inneholder structer for Oppgaven.


char buffer [2048];
int OwnAddress = -1;
int BasePort = -1;

char TCPBuffer [2048];


// Self-management prototyper
void initializeNode(int, char **);

// Rutertabell og TCP-relaterte prototyper.
void printRoutingTable(void);
int isDestinationInRoutingTable(int id);
int getNextHopByDestination(int dest);
void fetchRoutingTableFromServer(int serverSocket);

// UDP relaterte prototyper.. 
void listenForUDPPackages(void);
int sendUDPPackagesThroughNetwork(void);
char * constructUDPPackage(int destination, int source, char * message);
void sendPackageToDesination(int nextHop, int destination, int source, char * message);



int neighbourCount = 0; // Antallet nabo-noder.
struct NodeInfo * neighbourNodes; // Array that holds references to all neighboring nodes and their weights.

struct RoutingTableNode ** routingTable; // Inneholder informasjon om alle nestehoppene denne noden er ansvarlig for.
int tableSize = 0;


int UDPserverSocket = -1;
struct sockaddr_in udpServerAdresse;

int main(int argc, char * argv[]){

	printf("Hello I'm a node.c! Amount of arguments: %d \n",argc);
	
	initializeNode(argc, argv);

	openTCPConnectionToRoutingServer();

    // Print some statistics to make sure initializtion of the NodeInfo struct
    // was done right.
    printNodeInfo();

	UDPserverSocket = openUDPServerConnection();

    if(UDPserverSocket != -1){
        printf("UDP ServerSocket was successfully created! Port: %d \n", BasePort+OwnAddress);
    }

    // Sleep to wait for all UDP-servers to finish up being created.
    sleep(1);


    if(OwnAddress == 1){

        int result = sendUDPPackagesThroughNetwork();

    }else{

      listenForUDPPackages();

    }

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


int sendUDPPackagesThroughNetwork(){


        ////  1) les Data.txt & legg inn i et array.

        FILE * dataFile = fopen("data.txt","r");

        while(!feof(dataFile)){
            char messageBuffer[2048];
            int destination = 0; 
        
            fscanf(dataFile, "%d", &destination);


            // This reads in a message char for char until \n   linebreak.
            char nextChar = fgetc(dataFile);
            int currReadIndex = 0;
            while(nextChar != '\n' && feof(dataFile) == 0) {
                //printf("readChar %c \n", nextChar);
                messageBuffer[currReadIndex] = nextChar;
                currReadIndex++;
                nextChar = fgetc(dataFile);
            }


            // Hvis destination nå er 0 betyr det at alle meldingene er lest fra data.txt.
            if(destination != 0){
                // Legg til 0 terminering må meldinge bufferen.
                messageBuffer[currReadIndex] = '\0';


                ////  2) Konstruér pakker
                printf("\nRead with fscanf dest: %d   message: %s  \n", destination, messageBuffer );    


                /// Get nextHop 
                int nextHop = getNextHopByDestination(destination);

                // construct the package.
                char * package = constructUDPPackage(destination, OwnAddress, &messageBuffer);

                // Use the print_pkt function to check it the package is correct.
                print_pkt(package);

                sendPackageToDesination(nextHop, destination, OwnAddress, &package);

                free(package);


               
            }else{

                // Bryt ut av fillesingen
                break;
            }

        }


        fclose(dataFile);


        // Skip \n linje-skift char'en som kommer like etter de 4 bytene som holder antall rutere.
        

        

        ///// 3) Send pakker av gårde til riktig node ved å følge routingTable.






       



    return -1;
}


char * constructUDPPackage(int destination, int source, char * message){
    // Format  2 bytes = packet length | dest address  | source address | messsage that must be '\0' terminated.

    // packageBuffer er 2048 bytes.

    printf("COntructing package: dest: %d    src: %d     msg:  %s",destination, source, message);
    printf("message to be sent: \n");

    int i; 
    for(i = 0; i < strlen(message) ; i++){
        printf("%c", message[i]);
    }

    printf("\n");

    int packageLength = strlen(message);

    char * pckg = malloc(2 + 2 + 2 + packageLength);

qwe qowiejqwoiej 
    memcpy(&pckg[0],&packageLength,2);
    memcpy(&pckg[2],&destination,2);
    memcpy(&pckg[4],&source,2);
    memcpy(&pckg[6],&message, packageLength);

    return pckg;




}


void sendPackageToDesination(int nextHop, int destination, int source, char * package){

     struct sockaddr_in udpClientAddress;

        int outSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if(outSocket < 0){
            printf("UDP-Connection from  %d -> %d failed!\n", OwnAddress, nextHop);
            exit(EXIT_FAILURE);
        }

        // Filling server information 
        udpClientAddress.sin_family = AF_INET; 
        udpClientAddress.sin_port = htons(BasePort + nextHop); 
        udpClientAddress.sin_addr.s_addr = INADDR_ANY;


        printf("Sent UDP-message from %d --> %d \n", OwnAddress , nextHop);
        printf("Package:  %s \n", package);



         /* sendto(sockfd, (const char *)hello, strlen(hello), 
          MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));  */ 

        
        int bytesSent = sendto(outSocket, package , strlen(package), MSG_CONFIRM, (struct sockaddr *) &udpClientAddress, sizeof(udpClientAddress));

        printf("%d bytes were sent:   message was:  %s  \n",bytesSent, package );


}

void parseCommandFromUDPSocket(){

}


void listenForUDPPackages(){
    // n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
    //             MSG_WAITALL, (struct sockaddr *) &servaddr, 
    //             &len); 

    if(OwnAddress == 11){

        char UDPBuffer [2048]; 
        // char quitString [5];
        // strcpy(quitString, "QUIT");
        
        int len = sizeof(struct sockaddr); 
        int bytesRecieved = recvfrom(UDPserverSocket, UDPBuffer , 2048 , MSG_CONFIRM, (struct sockaddr *) &udpServerAdresse, &len);
        printf("\nbytesRecieved = %d   UDPBuffer value: %s \n", bytesRecieved, UDPBuffer );

        //// Parse UDP package.

    }
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







