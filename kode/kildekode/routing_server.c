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

struct NodeSocket ** nodeSockets;
int currentNodeSocketCount = 0;






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


int initializeTCPServer(){


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

            int bytesRecieved = recv(client_socket, &clientResponseBuffer, sizeof(clientResponseBuffer),0);

            if(bytesRecieved > 0){

                printf("%d bytes Recieved!! : Recieved message from socket: %d   message:%s\n", bytesRecieved, client_socket, clientResponseBuffer);


                // Decode data from socket.
                struct NodeSocket * currSock = getNodeSocketBySocketId(nodeSockets, currentNodeSocketCount, client_socket);

                // Copy in the ID of the node.
                memcpy(&currSock->nodeID, &clientResponseBuffer, sizeof(int));
                int readIndex = sizeof(int);

                // Count all the weights:
                while(clientResponseBuffer[readIndex] != '\0'){
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
                while(clientResponseBuffer[readIndex] != '\0'){

                    // Allocate dynamic space for a struct NodeInfo! To holde Weight / Edges.
                    currSock->nodes[curEdgeWeightIndex] = malloc( sizeof(struct NodeInfo) );

                    int to     =  clientResponseBuffer[readIndex];
                    readIndex += 4; 
                    int weight =  clientResponseBuffer[readIndex];
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
                int i;
                for(i = 0; i < 2048 ; i++ ){
                    clientResponseBuffer [i] = 0;
                }



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

    printf("All Edges / Weights \n (nodeID)  ---- weight --->  (nodeTargetID)  \n");
    printAllEdgesAndWeights(nodeSockets, currentNodeSocketCount);



    // When the code reaches this point all the nodes have successfully communicated
    // with the routing_server.



    // 1) CREATE SUFFICIENT STORAGE FOR SHORTEST PATHS.

    // In my system this is stored in struct NodeSocket.
    // But we need to allocate memory for the paths! ;) 
    int i; 
    for(i = 0; i < currentNodeSocketCount ; i++) {
        nodeSockets[i]->pathFrom1 = malloc(sizeof(int) * currentNodeSocketCount);

        int j;
        for(j = 0; j < currentNodeSocketCount ; j++){
            nodeSockets[i]->pathFrom1[j] = (int)(i * j);
            printf("nodesockets[i]->pathFRom1[j] = %d", nodeSockets[i]->pathFrom1[j]);
        }
        printf("\n");
    }

    

    // Now we need to calculate and send back the Shortest Paths.
    // From node 1 to all other nodes.
    



    // 2) Calculate shortest paths and store paths in NodeSockets.
    calculateDijkstrasShortestPaths(nodeSockets);  
    







    

}


void freeAllMemory(){

    // Free up dynamically allocated memory from nodeSockets.
    free(nodeSockets);
    int i;
    for(i = 0; i < currentNodeSocketCount ; i++){


        // TODO 1) MÅ FRIGJØRE nodeSockets[i].nodes!"!#=)"!(#!"=)#((#=)!")


        // TODO 2) MÅ FRIGJØRE nodesockets[i].nodePathForm1 !=")#(!"=)#( )


        // Til slutt frigjør pekeren selv.
        free(nodeSockets[i]);
    }

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

}




void calculateDijkstrasShortestPaths(struct NodeSocket * nodes [],){


    int Q[currentNodeSocketCount];
    
    // 1)  Calculate the Routing Table for the NodeSocket.
    int dist[currentNodeSocketCount];
    int prev[currentNodeSocketCount];

    int i ; 
    for(i = 0; i < currentNodeSocketCount; i++){
        dist[i] = INT_MAX;
        prev[i] = -1;
        Q[i] = i;
    }
    dist[1] = 0;


    while()
    /*

     from Wikipedia: ../wiki/Dijkstra%27s_algorithm
        create vertex set Q
 
        for each vertex v in Graph:             
            dist[v] ← INFINITY                  
            prev[v] ← UNDEFINED                 
            add v to Q                      
        dist[source] ← 0                        
      
        while Q is not empty:
            u ← vertex in Q with min dist[u]    
                                               
            remove u from Q 
          
            for each neighbor v of u:           // only v that are still in Q
                alt ← dist[u] + length(u, v)
                if alt < dist[v]:               
                    dist[v] ← alt 
                    prev[v] ← u 
  

    */


    // 2) Store the Route in each NodeSockets-pathFrom1 array.




}

void calculateRoutingTableForSocket(int nodeID){

    // RoutingTable Structure:
    // from    |  to        |   .... |  from      |    to        | '\0'
    // 4 bytes |  4 bytes   |        |  4 bytes   |    4 bytes   |

}


void printAllEdgesAndWeights(struct NodeSocket * sockets [], int len){
    int i = 0, j = 0;
    for (; i < len ; i++){
        for(j = 0; j < sockets[i]->nodeCount ; j++){
            printf(" (%d)  ----- %d ------>  (%d)  \n", sockets[i]->nodes[j]->from, sockets[i]->nodes[j]->weight, sockets[i]->nodes[j]->to);
        }
    }  

}











