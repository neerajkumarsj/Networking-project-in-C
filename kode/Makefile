CCFLAGS= -g 

all: routing_server node

routing_server: kildekode/routing_server.c kildekode/IN2140Networking.h print_lib/print_lib.h
	gcc kildekode/routing_server.c  print_lib/print_lib.c $(CCFLAGS) -o routing_server

node: kildekode/node.c kildekode/IN2140Networking.h print_lib/print_lib.h
	gcc kildekode/node.c print_lib/print_lib.c $(CCFLAGS) -o node


run: all
	bash run_1.sh 2020

run1: all
	bash run_1.sh 4020

run2: all
	bash run_2.sh 5020
	

clean:
	rm -f routing_server
	rm -f node
	rm logfile.txt
	rm -r logs/

