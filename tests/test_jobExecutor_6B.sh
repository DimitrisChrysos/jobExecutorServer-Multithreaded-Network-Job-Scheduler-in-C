source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
sleep 1
./bin/jobCommander $SERVER_HOST $PORT setConcurrency 4
sleep 1
./bin/jobCommander $SERVER_HOST $PORT poll
sleep 1
./bin/jobCommander $SERVER_HOST $PORT stop job_6
sleep 1
./bin/jobCommander $SERVER_HOST $PORT poll
sleep 1
./bin/jobCommander $SERVER_HOST $PORT exit