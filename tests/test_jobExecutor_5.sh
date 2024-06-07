source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST 2035 setConcurrency 2
sleep 1
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 5 &
sleep 1
./bin/jobCommander $SERVER_HOST 2035 poll
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 12 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 4 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 6 &
sleep 1
./bin/jobCommander $SERVER_HOST 2035 poll
sleep 1
./bin/jobCommander $SERVER_HOST 2035 exit
