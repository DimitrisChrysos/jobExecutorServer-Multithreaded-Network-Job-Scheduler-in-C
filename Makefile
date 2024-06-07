# To run the Makefile, type "make" at the terminal. 
# Now open two terminals, using split option:
# 1. On the 1st termnial: ./bin/jobExecutorServer [portnum] [bufferSize] [threadPoolSize]
# 	for example:	./bin/jobExecutorServer 8012 8 5
# 2. On the 2nd terminal:
# 	a) ./bin/jobCommander [serverName] [portNum] [jobCommanderInputCommand]
#		for example:	./bin/jobCommander linux10.di.uoa.gr 8012 exit
#	b) run a bash script using: bash tests/[bash script name]
# 		for example:	bash tests/tests/test_jobExecutor_1.sh

.PHONY: clean

everything:
	gcc -o bin/jobCommander src/jobCommander.c src/queue.c -g
	gcc -o bin/jobExecutorServer src/jobExecutorServer.c src/queue.c src/ServerCommands.c -lpthread -g
	gcc -o bin/progDelay src/progDelay.c -g

clean:
	rm -f bin/jobCommander
	rm -f bin/jobExecutorServer
	rm -f bin/progDelay