jobCommander:
	gcc -o jobCommander jobCommander.c queue.c
	gcc -o jobExecutorServer jobExecutorServer.c queue.c ServerCommands.c
	gcc -o progDelay progDelay.c

clean:
	rm -f jobCommander
	# rm -f jobExecutorServer
	# rm -f jobExecutorServer.txt