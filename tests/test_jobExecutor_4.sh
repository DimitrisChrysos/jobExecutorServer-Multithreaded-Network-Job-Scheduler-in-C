source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 20 &
sleep 1
./bin/jobCommander $SERVER_HOST 2035 poll
./bin/jobCommander $SERVER_HOST 2035 exit
