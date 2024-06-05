./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 10 &
sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 setConcurrency 4
sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 poll
sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 stop job_6
sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 poll
# sleep 1
# ./bin/jobCommander linux01.di.uoa.gr 2035 exit