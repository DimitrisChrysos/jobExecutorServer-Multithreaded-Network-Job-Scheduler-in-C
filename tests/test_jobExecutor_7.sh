source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander $SERVER_HOST 2035 stop job_1
./bin/jobCommander $SERVER_HOST 2035 stop job_2
./bin/jobCommander $SERVER_HOST 2035 exit