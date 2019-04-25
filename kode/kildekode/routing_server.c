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

char TCPBuffer [2048];

struct NodeSocket ** nodeSockets;
int currentNodeSocketCount = 0;


void initializeTCPServer(void);
void printBuffer(char * arr, int len);
void printRoutingTables(struct NodeSocket * sockets []);
void savePathRecursivelyToIntArray(int * prev  , int index, int * destinationArray, int currIndex);


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


    initializeTCPServer();   

	// Les data.txt.

    // Lagre nodene i en datastruktur.




    // Åpne en Socket som lytter på en bestemt Port
    // Og vent til Alle de N node-programmene har koblet seg til.

    // Så må jeg bygge opp en graf og kalkulere dijkstras-algoritme på den.
	

	




	return 0;
}


int nodeConnectionsCountSoFar = 0;


void initializeTCPServer(){


    // Lag en NodeSocket liste som alle nodene kan lagres i. ( Vil brukes som Graf for Dijsktras senere ).
    
    nodeSockets = malloc(sizeof(struct NodeSocket *) * N);
    


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

    	int client_socket = accept(ruterServerSocket, NULL, NULL);
    	printf("accept() called!\n");

        if(client_socket == -1){

            printf("Accept() called but client_socket has a failure value.\n");

        }else{
            printf("A TCP-client connected!!\n client_socket_id = %d \n", client_socket);

            // Handle Storage of the client structre here:
            nodeSockets[currentNodeSocketCount] = malloc(sizeof(struct NodeSocket) );
            nodeSockets[currentNodeSocketCount]->socketID = client_socket;
            nodeSockets[currentNodeSocketCount]->nodes = NULL;
            currentNodeSocketCount ++;

            int bytesRecieved = recv(client_socket, &TCPBuffer, sizeof(TCPBuffer),0);

            if(bytesRecieved > 0){

                printf("%d bytes Recieved!! : Recieved message from socket: %d   message:%s\n", bytesRecieved, client_socket, TCPBuffer);


                // Decode data from socket.
                struct NodeSocket * currSock = getNodeSocketBySocketId(nodeSockets, currentNodeSocketCount, client_socket);

                // Copy in the ID of the node.
                memcpy(&currSock->nodeID, &TCPBuffer, sizeof(int));
                int readIndex = sizeof(int);

                // Count all the weights:
                while(TCPBuffer[readIndex] != '\0'){
                    readIndex += sizeof(int) * 2;
                }
                int amountOfEdgeWeights = readIndex / 4 / 2;
                printf("amountOfEdgeWeights %d \n", amountOfEdgeWeights);



                // Create dynamcally allocated struct NodeInfo pointer(!) Array.
                currSock->nodes = malloc(sizeof(struct NodeInfo * ) * amountOfEdgeWeights);
                currSock->nodeCount = amountOfEdgeWeights;


                readIndex = sizeof(int);
                // Copy in all the weights.
                int curEdgeWeightIndex = 0;
                while(TCPBuffer[readIndex] != '\0'){

                    // Allocate dynamic space for a struct NodeInfo! To holde Weight / Edges.
                    currSock->nodes[curEdgeWeightIndex] = malloc( sizeof(struct NodeInfo) );

                    int to     =  TCPBuffer[readIndex];
                    readIndex += 4; 
                    int weight =  TCPBuffer[readIndex];
                    readIndex += 4;
                    printf("nodeID[%d] has Edge/weight;  to:  %d     weight:    %d \n",currSock->nodeID, to,weight );

                    currSock->nodes[curEdgeWeightIndex]->OwnAddress = currSock->nodeID;
                    currSock->nodes[curEdgeWeightIndex]->from = currSock->nodeID;
                    currSock->nodes[curEdgeWeightIndex]->to = to;
                    currSock->nodes[curEdgeWeightIndex]->weight = weight;


                    curEdgeWeightIndex++;
                }


                // Store the weights in the correct place.


                // Clean up the buffer.
                clearTCPBuffer();



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

            

        }




        // RECEPTION OF DATA.



    	// recv(ruterServerSocket, char buffer[248], sizeof(buffer), 0);
    	
        


        // Store the Socket connection and the data from the Socket.



        // Increase the amount of stored Data.
        nodeConnectionsCountSoFar++;


    }

    printf("All nodes currently in system: \n");
    printAllNodeSockets(nodeSockets, currentNodeSocketCount);

    
    printAllEdgesAndWeights(nodeSockets, currentNodeSocketCount);



    // When the code reaches this point all the nodes have successfully communicated
    // with the routing_server.



    // 1) CREATE SUFFICIENT STORAGE FOR SHORTEST PATHS.

    // In my system this is stored in struct NodeSocket.
    // But we need to allocate memory for the paths! ;) 

    printf("Allocating memory for storing Paths in each NodeSocket.");
    int i; 
    for(i = 0; i < currentNodeSocketCount ; i++) {
        nodeSockets[i]->pathFrom1 = malloc(sizeof(int) * currentNodeSocketCount);

        int j;
        for(j = 0; j < currentNodeSocketCount ; j++){
            nodeSockets[i]->pathFrom1[j] = -1;
           // printf("nodesockets[i]->pathFRom1[j] = %d", nodeSockets[i]->pathFrom1[j]);
        }
        printf("\n");
    }

    

    // Now we need to calculate and send back the Shortest Paths.
    // From node 1 to all other nodes.
    



    // 2) Calculate shortest paths and store paths in NodeSockets.
    FindDijkstrasShortestPaths(nodeSockets, 1);  
    

    CalculateRoutingTableForAllNodeSockets(nodeSockets);

    // Print Routing Tables
    printRoutingTables(nodeSockets);


    sendBackRoutingTablesToAllNodeSockets(nodeSockets);

    freeAllAllocatedMemory();
    

}


void printRoutingTables(struct NodeSocket * nodeSockets []){
    int i;
    for(i = 0; i < N; i++){
        int j; 
        for(j = 0; j < N; j++){
            printf("nodeId: %d -> [%d] = %d\n",nodeSockets[i]->nodeID, nodeSockets[j]->nodeID, nodeSockets[i]->routingTable[j]);
        }
    }
}

void sendBackRoutingTablesToAllNodeSockets(struct NodeSocket * nodeSockets []) {


    printf("\n\nSending back routing Tables over TCP: \n")  ;

	// RoutingTable Structure:
    // len        | from    |  to        |   .... |  from      |    to        | '\0'
    // 4 bytes    | 4 bytes |  4 bytes   |        |  4 bytes   |    4 bytes   |

	char TCPBuffer[2048]; 
	int bufferIndex = 0;
    int i; 
	for( i = 0 ; i < N; i++){

        printf("\nSending back Routing Table for nodeID:  %d\n", nodeSockets[i]->nodeID);
		clearTCPBuffer();
		bufferIndex = sizeof(int);
		
     	int j, validPairCount = 0;
		for(j = 0; j < N ; j++){
			
             
			// Copy routing-data into the buffer that will be sent over TCP back to the Node.
			if(nodeSockets[i]->routingTable[j] != -1 && nodeSockets[j]->nodeID != 1){
				// memcpy(dest, src, size_t)
				memcpy(&TCPBuffer[bufferIndex], &nodeSockets[j]->nodeID, sizeof(int)); 	
				bufferIndex += sizeof(int);
				memcpy(&TCPBuffer[bufferIndex], &nodeSockets[i]->routingTable[j], sizeof(int)); 	
                bufferIndex += sizeof(int);
                validPairCount++;
			}
			
		}

        // Finally store the length (in bytes) of the remaining data.
        int validDataLength = validPairCount * 4 * 2;
        char nullChar = '\0';
        memcpy(&TCPBuffer, &validDataLength, sizeof(int) );
        memcpy(&TCPBuffer[bufferIndex], &nullChar, 1);

        printf("DataSize: %d bytes. + 4 bytes for this int.\n", validDataLength) ;
        printBuffer(TCPBuffer,2048);

		// int bytesSent = send(socketToRouter,nodeInfoTCPBuffer, sizeof(nodeInfoTCPBuffer),0);
		int bytesSent = send(nodeSockets[i]->socketID, &TCPBuffer, sizeof(TCPBuffer), 0);

	}

}


void clearTCPBuffer(){
	int i;
    for(i = 0; i < 2048 ; i++ ){
        TCPBuffer [i] = 0;
    }

}


void printBuffer(char * arr, int len){
    int i;
    for(i = 0; i < len-1; i++){
        printf("%d ",arr[i]);
    }
}

void freeAllAllocatedMemory(){

    // Free up dynamically allocated memory from nodeSockets.
    
    int i;
    for(i = 0; i < currentNodeSocketCount ; i++){


        // TODO 1) MÅ FRIGJØRE nodeSockets[i].nodes!"!#=)"!(#!"=)#((#=)!")


        // TODO 2) MÅ FRIGJØRE nodesockets[i].nodePathForm1 !=")#(!"=)#( )


        // Til slutt frigjør pekeren selv.
        
        int j; 
        for(j = 0; j < nodeSockets[i]->nodeCount; j++){
        	free(nodeSockets[i]->nodes[j]);
        	free(nodeSockets[i]->pathFrom1);
        	free(nodeSockets[i]->routingTable);
        }
        free(nodeSockets[i]);
    }
    free(nodeSockets);

}


void printAllNodeSockets(struct NodeSocket * sockets [], int len ){

    int i = 0;
    for (; i < len ; i++){

        printf("\nNodeID   : %d ", sockets[i]->nodeID);
        printf("\nSocketID : %d ", sockets[i]->socketID);

        printf("\n\n All neighbours\n");
        int j = 0;
        for(j = 0; j < sockets[i]->nodeCount ; j++){
            printf("From: %d  to: %d  weight: %d\n",  sockets[i]->nodes[j]->from, sockets[i]->nodes[j]->to, sockets[i]->nodes[j]->weight);
        }


        // Print path if there is one.
        printf("\nprint Path: \n");
        if(sockets[i] != NULL) {
	        if(sockets[i]->pathFrom1 != NULL){
		        for(j = 0; sockets[i]->pathFrom1[j] != -1; j++){
		        	printf("%d ",sockets[i]->pathFrom1[j]);
		        }
	        }
    	}
        
        printf("\n\n");
    
    }  

}



struct NodeSocket * getNodeSocketBySocketId(struct NodeSocket * sockets [], int len,  int socketID){
    int i;
    for(i = 0; i < len; i++  ){
        if(sockets[i]->socketID == socketID){
            return sockets[i];
        }
    }

    return NULL;

}


#define INFINITE_MAXIMA 99392392 // Egentlig infinity, trengte bare å gjøre koden litt morsom.

void FindDijkstrasShortestPaths(struct NodeSocket * nodes [], int startNode){

	printf("Dijkstra has started!!");
	int N = currentNodeSocketCount;
    int Q[N];
    

    int dist[N]; // Denne vil holde avstanden til de ulike nodene.
    int prev[N]; // Denne vil holde forrige data.

    int i ;   /// Here i denotes the index of the various nodes in the NodeSocket nodes[] array.
    for(i = 0; i < N; i++){
        dist[i] = INFINITE_MAXIMA;
        prev[i] = -1;
        Q[i] = 1;
    }

    int startNodeIndex = getIndexOfNodeSocketWithNodeID(startNode);
    printf("\n\nstartNode: %d     startNodeINdex : %d \n\n", startNode, startNodeIndex);

    // Avstanden til startNoden vil alltid være 0.
    dist[startNodeIndex] = 0;


    // Er Q tomt enda? Hvis ikke kalkulér path.
    while(isArrayEmpty(&Q,N) == -1){
    	printf("isArrayEmpty  %d \n", isArrayEmpty(&Q,N));

    	// Find min dist[u];
    	int u = findIndexInQWithMinDist(&Q, &dist, N);
    	
    	Q[u] = -1;


    	// Bla igjennom alle naboene i noden vi jobber med for øyeblikket.
    	int i, alt;
    	struct NodeSocket * currentNode = nodes[u];	
    	for (i = 0; i < currentNode->nodeCount; i++ ){

    		int v = getIndexOfNodeSocketWithNodeID(currentNode->nodes[i]->to);

    		// If the element is already taken out of Q - don't bother.    	
    		int res = isIndexInArrayEmpty(&Q,N,v);
    		if(res == 1)
    			continue;
    		


    		int distanceBetweenNodes = lengthBetweenNodes(currentNode, i);
    	    alt = dist[u] + distanceBetweenNodes;

    	    
            if(alt < dist[v]){
            	dist[v] = alt;
            	prev[v] = u;
            }

    	}

    }


    // 2) Store the shortest path to 1 in each NodeSockets[i]->pathFrom1 array.
    for (i = 1; i < N; i++) { 
        savePathRecursivelyToIntArray(prev, i, nodes[i]->pathFrom1, 0); 
        nodes[i]->shortestDistTo1 = dist[i];
        // Reverse the array since it in fact stores the values backwards.
        reversePathArray(nodes[i]->pathFrom1, N);
    } 

    printAllNodeSockets(nodes, N);


}

int isIndexInArrayEmpty(int * arr, int len, int index){
	// If the index is out of Bounds return -1 
	if(index > len)
		return 1;

	// If the value in the array at index is -1 return true!
	// I define a value of -1 to be empty.
	if(arr[index] == -1){
		return 1;
	}else{
		return -1;
	}

}

int lengthBetweenNodes(struct NodeSocket * node, int index){
    return node->nodes[index]->weight;
}

// Returns the Index of the element with the least distance.
int findIndexInQWithMinDist(int * Q, int * dist, int len){
	int i;
	int minDist = INFINITE_MAXIMA;
	int indexOfMinDist = -1;
	for(i = 0; i < len; i++){

		// Check that the element is in Q and then check if the dist is smaller.
		if(Q[i] != -1 && dist[i] <= minDist){
			minDist = dist[i];
			indexOfMinDist = i;
		}
	}
	
	return indexOfMinDist;
}
	
// If One of the elements in the array is -1 ( which I use to denote emptiness )
// Then the function returns -1 which means. NO its not empty.
int isArrayEmpty(int * arr , int len){
	int i; 
	for(i = 0; i < len; i++){
		if(arr[i] != -1){
			return -1;
		}
	}

	return 1;
}

void CalculateRoutingTableForAllNodeSockets(struct NodeSocket * nodes []){

	// Routing table is stored inside each NodeSocekt int * 


	int i; 
    for(i = 0; i < N; i++){

    	// Create routing Table.
    	nodes[i]->routingTable   = malloc(sizeof(int) * N);

    	int j;

        // Set the first node which tells us about the startingNode
	    for(j = 1; j < N; j++){

	    	int k;
	    	// Set default value for the routing table.
	    	nodes[i]->routingTable[j] = -1;

	    	// Iterate through all the shortest paths of all nodes.
	    	for(k = 0; k < N; k++){
	    		// If any of the nodes include this node's ID
	    		if(nodes[j]->pathFrom1[k] == nodes[i]->nodeID){
	    			// printf("MATCH FOUND %d , %d ",nodes[j]->pathFrom1[k], nodes[i]->nodeID );
	    			if(nodes[j]->pathFrom1[k+1] != -1){
	    				nodes[i]->routingTable[j] = nodes[j]->pathFrom1[k+1];
	    			}
	    		}
	    	}
	    	
	    	// Hvis denne noden er nevnt i en annen nodes path.
	    	// Må denne noden's routingTable vite om det.
	    }

    }

}


int getIndexOfNodeSocketWithNodeID(int nodeID){
	int i ;
	for(i = 0; i < N;i++){
		if(nodeSockets[i]->nodeID == nodeID)
			return i;
	}
	// return -1 if it failed.
	return -1;
}

void printAllEdgesAndWeights(struct NodeSocket * sockets [], int len){
	printf("All Edges / Weights \n (nodeID)  ---- weight --->  (nodeTargetID)  \n");
    int i = 0, j = 0;
    for (; i < len ; i++){
        for(j = 0; j < sockets[i]->nodeCount ; j++){
            printf(" (%d)  ----- %d ------>  (%d)  \n", sockets[i]->nodes[j]->from, sockets[i]->nodes[j]->weight, sockets[i]->nodes[j]->to);
        }
    }  

}


void savePathRecursivelyToIntArray(int * prev  , int index, int * destinationArray, int currIndex) { 
      
    if(prev[index] == -1){
        // Add the last index to the Path.
        destinationArray[currIndex] = 1;
        return;
    }
	destinationArray[currIndex] = nodeSockets[index]->nodeID;
	currIndex++;
    savePathRecursivelyToIntArray(prev, prev[index], destinationArray, currIndex); 
    
    return;
} 
  
int reversePathArray(int * array, int len ){

	int temp[len];
	int i;
	int amountOfNodesInPath = 0;
	for(i = 0; i < len; i++){
		if(array[i] != -1){
			temp[i] = array[i];
			amountOfNodesInPath++;
		}
	}

	for(i = 0; i < amountOfNodesInPath-1; i++){
		array[i] = temp[amountOfNodesInPath-i-1];
	}
	array[amountOfNodesInPath-1] = temp[0];
}








