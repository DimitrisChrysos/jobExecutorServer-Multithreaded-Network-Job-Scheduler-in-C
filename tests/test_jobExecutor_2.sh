source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 4
./bin/jobCommander $SERVER_HOST $PORT exit