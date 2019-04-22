#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> // For å kunne stenge en socket med close().
#include <sys/socket.h> // For det standardisérte socket-API'et.
#include <sys/types.h> // For noen av typene som kreves for å få sockets til å funke.

#include <netinet/in.h> // Inneholder noen av addresse structene som man bruker med sockets.


char buffer [2048];
int OwnAddress = -1;
int BasePort = -1;

void initializeNode(int, char **);

struct NodeInfo{
	int OwnAddress;
	int BasePort;
	int from;
	int to;
	int weight;
};

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

            int currentNeighbourIndex = i - neighbourCount;
			// Så må vi allokere minne til structen som skal lagres.
			neighbourNodes[currentNeighbourIndex].from = OwnAddress;

			// Leser inn informasjon i structen fra argumentet.
			sscanf(argList[i], "%d:%d", &neighbourNodes[currentNeighbourIndex].to, &neighbourNodes[currentNeighbourIndex].weight);

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
    	// OwnAdress\nEdge:Weight\nEdge:Weight\nEdge:Weight...Edge:Weight\0

    	// char edgeWeightList [2048];
    	// int curIndex = 0;
     //    int i;
     //    char toBuff[64];
     //    char weightBuff[64];
    	// for(i = 0; i < neighbourCount; i++){
            
          
     //        int toDataLen = 1 + snprintf(toBuff,"%d:",neighbourNodes[i].to);
     //        int weightDataLen 1 + snprintf(weightBuff,"%d\n",neighbourNodes[i].weight);

     //        // Copy nodeID ( NodeInfo.to )  from the the current neightbouringNode into the Edge/Weight-Buffer.
     //        memcpy(edgeWeightList[curIndex], toBuff, toDataLen);
     //        curIndex += toDataLen;

     //        // Copy weight ( NodeInfo.weight ) from the the current neightbouringNode into the Edge/Weight-Buffer.
     //        memcpy(edgeWeightList[curIndex], weightBuff, weightDataLen);
     //        curIndex += weightDataLen;

            
    	// }



    	char nodeInfoTCPBuffer [2048] = "123123";
    	// Skriver direkte til en char * 
    	//snprintf(nodeInfoTCPBuffer, sizeof(buffer), "%d\n%s", OwnAddress, edgeWeightList)

    	



    	// #2 Send informasjonen for denne Noden.

    	// eksempel
    	// send(socketToRouter, char buffer[248], sizeof(buffer), 0) 
        send(socketToRouter,nodeInfoTCPBuffer, sizeof(nodeInfoTCPBuffer),0);







        // #3 Vent på ruter-data fra hovedprogrammet




        // #4 Lagre rute-tabellen 





        // #5 Terminér TCP-tilkoblingen med rute_serveren.
        close(socketToRouter);

    }else{

    	// Error!
    	printf("Det skjedde en feil ved opprettelsen av TCP-router-klient-socketen!");
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







