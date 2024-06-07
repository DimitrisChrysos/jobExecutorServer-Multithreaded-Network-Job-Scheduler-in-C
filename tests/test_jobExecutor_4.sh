source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 20 &
sleep 1
./bin/jobCommander $SERVER_HOST $PORT poll
./bin/jobCommander $SERVER_HOST $PORT exit
