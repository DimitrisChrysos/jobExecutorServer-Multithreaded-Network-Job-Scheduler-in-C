source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST $PORT poll
./bin/jobCommander $SERVER_HOST $PORT exit
