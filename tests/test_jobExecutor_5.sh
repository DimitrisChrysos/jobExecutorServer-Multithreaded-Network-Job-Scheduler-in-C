./bin/jobCommander linux01.di.uoa.gr 2035 setConcurrency 5
# sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
# sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
# sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 5 &
# sleep 1
# ./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 2
# ./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 3
# ./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 2
./bin/jobCommander linux01.di.uoa.gr 2035 poll
# sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 exit
