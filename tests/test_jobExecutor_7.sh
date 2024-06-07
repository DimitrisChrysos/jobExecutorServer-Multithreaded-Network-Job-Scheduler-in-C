source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST $PORT issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST $PORT stop job_1
./bin/jobCommander $SERVER_HOST $PORT stop job_2
./bin/jobCommander $SERVER_HOST $PORT exit