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
./bin/jobCommander $SERVER_HOST $PORT stop job_15
./bin/jobCommander $SERVER_HOST $PORT stop job_6
sleep 1
./bin/jobCommander $SERVER_HOST $PORT poll
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 3 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 5 &
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 5 &
sleep 1
./bin/jobCommander $SERVER_HOST $PORT setConcurrency 3
./bin/jobCommander $SERVER_HOST $PORT poll
sleep 10
echo AFTER 10 SECONDS
./bin/jobCommander $SERVER_HOST $PORT poll
./bin/jobCommander $SERVER_HOST $PORT setConcurrency 10
./bin/jobCommander $SERVER_HOST $PORT poll
sleep 1
./bin/jobCommander $SERVER_HOST $PORT exit