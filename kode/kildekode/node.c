#include <stdio.h>
#include <stdlib.h>


char buffer [2048];
int OwnAddress = -1;
int BasePort = -1;

struct NodeInfo{
	int OwnAddress;
	int BasePort;
};

struct NodeInfo n1;



int main(int argc, char* argv[]){

	printf("Hello I'm node.c ! Amount of arguments: %d \n",argc);
	int i;
	printf("n1.OwnAddress: %d",n1.OwnAddress);
	printf("n1.BasePort: %d",n1.BasePort);

	for(i = 0; i < argc ; i++){
		printf("argument %d: %s \n",i, argv[i]);


		if( i == 1 ){
			BasePort = atoi(argv[i]);
		}

		if( i == 2 ){
			OwnAddress = atoi(argv[i]);
		}


		// Store Neighbooring Nodes + Weights.
		if( i >= 3){

		}

	}


	printNodeInfo();

	return 0;
}


int printNodeInfo(){
	printf("Node Information:::::\n----------------\n");
	printf("OwnAddress: %d \n", OwnAddress);
	printf("BasePort  : %d \n", BasePort);
	printf("\n");
}






