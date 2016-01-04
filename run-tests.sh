#!/bin/bash

# Use colors if output is terminal
if [ -t 1 ]; then
	COLOR_RED="\e[0;31m"
	COLOR_OFF="\e[0m"
fi

DIR=$(dirname $(readlink -f $0))
export TESTDIR="$DIR/tests"
export DATADIR="$DIR/test-data"
export BIN="$DIR/obj-magic"
export TEMPDIR=`mktemp -dt obj-magic-testing.XXXXXXXX`

LOGFILE="$TEMPDIR/test.log"
FAILS=0

echo "Starting tests @ $HOSTNAME on `date -R`"
echo "Starting tests @ $HOSTNAME on `date -R`" > "$LOGFILE"

cd "$TESTDIR"
function run_tests {
	for test in `ls`; do
		echo -n "Testing $test..."
		echo >> "$LOGFILE"
		echo "Output for test $test" >> "$LOGFILE"
		echo >> "$LOGFILE"
		bash "$test" >> "$LOGFILE" 2>&1
		if [ $? -eq 0 ]; then
			echo "OK"
		else
			echo -e "${COLOR_RED}FAIL${COLOR_OFF}"
			FAILS=$(($FAILS + 1))
		fi
	done

	# Print fail count and delete temp files on success
	if [ "$FAILS" -gt 0 ]; then
		echo -e "${COLOR_RED}${FAILS} failed tests${COLOR_OFF}"
		echo "Temp files kept in $TEMPDIR"
	else
		echo "All tests successful"
		rm -rf "$TEMPDIR"
	fi
	echo -n "Testing completed, elapsed time: "
	return $FAILS
}

time run_tests
