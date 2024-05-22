# To run the following, type the following commands in a terminal, inside this directory: 
# 1. make clean
# 2. make
# Now open two terminals, using split option
# 1. On the left: ./jobExecutorServer [portnum] [bufferSize] [threadPoolSize]
# 2. On the right: ./jobCommander [serverName] [portNum] [jobCommanderInputCommand]

jobCommander:
	gcc -o jobCommander jobCommander.c queue.c
	# gcc -o jobExecutorServer jobExecutorServer.c queue.c ServerCommands.c
	# gcc -o progDelay progDelay.c

clean:
	rm -f jobCommander
	# rm -f jobExecutorServer
	# rm -f jobExecutorServer.txt