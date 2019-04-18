CCFLAGS= -g 

all: routing_server node

routing_server: routing_server.c
	gcc routing_server.c $(CCFLAGS) -o routing_server

node: node.c
	gcc node.c $(CCFLAGS) -o node


run: all
	bash run.sh

clean:
	rm -f routing_server
	rm -f node
