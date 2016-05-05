#!/bin/bash

START=$(date "+%s")

ONE_DAY_FLAG=0
TWO_DAY_FLAG=0

# one dir's size = 45GB
# all disk size = 45GB * dir_count
DIR_COUNT=100

test_writev_rm ()
{
    # write day1's files
    ./write.sh "day1" $DIR_COUNT &

	while [ 1 ]
	do
		NOW=$(date "+%s")
		TIME=$((NOW-START))
        # if TIME more than 24 hours, then remove day1's files, write day2's files
        if (( ($TIME >= 86400) && ($ONE_DAY_FLAG == 0) )); then
            echo "time already is more than 24 hours."
            echo "Now delete day1's files, write day2's files"
            ONE_DAY_FLAG=1
			./remove.sh "day1" $DIR_COUNT &
			./write.sh "day2" $DIR_COUNT &
        elif (( ($TIME >= 172800) && ($TWO_DAY_FLAG == 0) )); then
            TWO_DAY_FLAG=1
            echo "time already is more than 48 hours."
            echo "Now delete day2's files, write day3's files"
			./remove.sh "day2" $DIR_COUNT &
			./write.sh "day3" $DIR_COUNT &
			wait && exit $?
		fi
        #echo "$TIME"
        sleep 1
	done
}

test_writev_rm

exit $?
