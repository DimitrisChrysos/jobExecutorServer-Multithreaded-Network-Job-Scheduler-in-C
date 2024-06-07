source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
sleep 1
./bin/jobCommander $SERVER_HOST 2035 setConcurrency 4
sleep 1
./bin/jobCommander $SERVER_HOST 2035 poll
sleep 1
./bin/jobCommander $SERVER_HOST 2035 stop job_15
./bin/jobCommander $SERVER_HOST 2035 stop job_6
sleep 1
./bin/jobCommander $SERVER_HOST 2035 poll
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 3 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 5 &
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 5 &
sleep 1
./bin/jobCommander $SERVER_HOST 2035 setConcurrency 3
./bin/jobCommander $SERVER_HOST 2035 poll
sleep 10
echo AFTER 10 SECONDS
./bin/jobCommander $SERVER_HOST 2035 poll
./bin/jobCommander $SERVER_HOST 2035 setConcurrency 10
./bin/jobCommander $SERVER_HOST 2035 poll
sleep 1
./bin/jobCommander $SERVER_HOST 2035 exit