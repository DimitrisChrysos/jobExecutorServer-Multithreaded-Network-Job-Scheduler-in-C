source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 4
./bin/jobCommander $SERVER_HOST 2035 exit