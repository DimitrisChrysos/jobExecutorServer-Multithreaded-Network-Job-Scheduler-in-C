source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST $PORT setConcurrency 2
sleep 1
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 5 &
sleep 1
./bin/jobCommander $SERVER_HOST $PORT poll
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 12 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 4 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 6 &
sleep 1
./bin/jobCommander $SERVER_HOST $PORT poll
sleep 1
./bin/jobCommander $SERVER_HOST $PORT exit
