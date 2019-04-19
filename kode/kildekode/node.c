#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> // For å kunne stenge en socket med close().
#include <sys/socket.h> // For det standardisérte socket-API'et.


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

	printf("Hello I'm node.c ! Amount of arguments: %d \n",argc);
	
	initializeNode(argc, argv);


	openTCPConnectionToRoutingServer();

	printNodeInfo();

	return 0;
}









void initializeNode(int argCount, char * argList[]){
	
    // If the arguments to this program are formatted correctly
    // argc - 3 should be the correct size of the Edge -> Weight Array.
    neighbourCount = argCount - 3;
    neighbourNodes = malloc(sizeof(struct NodeInfo) * neighbourCount);

    int i;
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


	return 0;
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


void parseCommandFromUDPSocket(){
	
}






