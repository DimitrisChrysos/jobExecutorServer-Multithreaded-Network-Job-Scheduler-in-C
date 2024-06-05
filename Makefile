# To run the Makefile, type "make" at the terminal. 
# Now open two terminals, using split option:
# 1. On the 1st termnial: ./bin/jobExecutorServer [portnum] [bufferSize] [threadPoolSize]
# 2. On the 2nd terminal:
# 	a) ./bin/jobCommander [serverName] [portNum] [jobCommanderInputCommand]
#	b) run a bash script using: bash tests/[bash script name]

.PHONY: clean

everything:
	gcc -o bin/jobCommander src/jobCommander.c src/queue.c
	gcc -o bin/jobExecutorServer src/jobExecutorServer.c src/queue.c src/ServerCommands.c -lpthread
	gcc -o bin/progDelay src/progDelay.c

clean:
	rm -f bin/jobCommander
	rm -f bin/jobExecutorServer
	rm -f bin/progDelay