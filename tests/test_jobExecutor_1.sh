source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST 2035 issueJob touch build/myFile.txt
ls build/myFile.txt
./bin/jobCommander $SERVER_HOST 2035 issueJob rm build/myFile.txt
ls build/myFile.txt
./bin/jobCommander $SERVER_HOST 2035 exit
