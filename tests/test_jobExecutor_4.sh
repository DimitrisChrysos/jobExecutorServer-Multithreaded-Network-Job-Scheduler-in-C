./bin/jobCommander linux01.di.uoa.gr 2035 issueJob ./bin/progDelay 20 &
sleep 1
./bin/jobCommander linux01.di.uoa.gr 2035 poll
./bin/jobCommander linux01.di.uoa.gr 2035 exit
