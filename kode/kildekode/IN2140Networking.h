

// Denne structen brukes for å lagre informasjon om 
// èn node og en kant den har samt vekten.
// Så en node med 3 kanter utgående i en graf, vil ha 3 av denne.
// Hvor både from & OwnAddress er id'en (navnet) til noden.
struct NodeInfo{
	int OwnAddress;
	int BasePort;
	int from;
	int to;
	int weight;
};



struct NodeSocket{
    int socketID;
    int nodeID;
    int nodeCount;
    struct NodeInfo ** nodes;
    int * pathFrom1;
};



void printAllNodeSockets(struct NodeSocket * sockets [], int len );
struct NodeSocket * getNodeSocketBySocketId(struct NodeSocket * sockets [], int len,  int socketID);
void calculateDijkstrasShortestPaths(struct NodeSocket * nodes [], int startNode);
void printAllEdgesAndWeights(struct NodeSocket * sockets [], int len);
int getIndexOfNodeSocketWithNodeID(int nodeID);