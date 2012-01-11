#!/bin/bash -e

if [ "x$1" = "x" ]; then
	echo "Parameter name to test required"
	exit 1
fi

TEST=$1
PARAM=$2

DIR=$(dirname $(readlink -f $0))
TESTDIR="$DIR/tests"
DATADIR="$DIR/test-data"
BIN="$DIR/obj-magic"
TESTFILE="$TESTDIR/$TEST"

INFILE="$DATADIR/square.obj"
REFFILE="$DATADIR/square-$TEST.obj"

$BIN --$TEST $PARAM "$INFILE" > "$REFFILE"

rm -f "$TESTFILE"

cat >> $TESTFILE << EOF
#!/bin/bash

INFILE="\$DATADIR/square.obj"
OUTFILE="\$TEMPDIR/$TEST.obj"
REFFILE="\$DATADIR/square-$TEST.obj"

\$BIN --$TEST $PARAM "\$INFILE" > "\$OUTFILE"

cmp -s "\$REFFILE" "\$OUTFILE"
exit \$?

EOF
