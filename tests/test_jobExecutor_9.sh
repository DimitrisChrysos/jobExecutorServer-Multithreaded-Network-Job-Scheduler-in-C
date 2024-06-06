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
./bin/jobCommander linux01.di.uoa.gr 2035 stop job_15
./bin/jobCommander linux01.di.uoa.gr 2035 stop job_6
sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 poll
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 3 &
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 5 &
./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 5 &
sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 setConcurrency 3
./bin/jobCommander linux01.di.uoa.gr 2035 poll
sleep 10
echo AFTER 10 SECONDS
./bin/jobCommander linux01.di.uoa.gr 2035 poll
./bin/jobCommander linux01.di.uoa.gr 2035 setConcurrency 10
./bin/jobCommander linux01.di.uoa.gr 2035 poll
sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 exit