CCFLAGS= -g 

all: routing_server node

routing_server: kildekode/routing_server.c
	gcc kildekode/routing_server.c $(CCFLAGS) -o routing_server

node: kildekode/node.c
	gcc kildekode/node.c $(CCFLAGS) -o node


run: all
	bash run.sh

clean:
	rm -f routing_server
	rm -f node
	rm -r logs/
