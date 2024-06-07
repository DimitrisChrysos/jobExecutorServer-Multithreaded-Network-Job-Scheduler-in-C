source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST $PORT issueJob touch build/myFile.txt
ls build/myFile.txt
./bin/jobCommander $SERVER_HOST $PORT issueJob rm build/myFile.txt
ls build/myFile.txt
./bin/jobCommander $SERVER_HOST $PORT exit
