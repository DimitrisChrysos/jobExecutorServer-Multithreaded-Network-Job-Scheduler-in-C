source ./tests/server_host.sh
./bin/jobCommander $SERVER_HOST 2035 issueJob ls -l /usr/bin/* /usr/local/bin/* /bin/* /sbin/* /opt/* /etc/* /usr/sbin/*
./bin/jobCommander $SERVER_HOST 2035 exit