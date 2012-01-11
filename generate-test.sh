#!/bin/bash -e

if [ "x$1" = "x" ]; then
	echo "Parameter name to test required"
	exit 1
fi

TEST=$1
PARAM=$2
if [ "$PARAM" ]; then
	PARAM_="_$PARAM"
fi

DIR=$(dirname $(readlink -f $0))
TESTDIR="$DIR/tests"
DATADIR="$DIR/test-data"
BIN="$DIR/obj-magic"
TESTFILE="$TESTDIR/$TEST$PARAM_"

REF="square-$TEST$PARAM_.obj"

$BIN --$TEST $PARAM "$DATADIR/square.obj" > "$DATADIR/$REF"

rm -f "$TESTFILE"

cat >> $TESTFILE << EOF
#!/bin/bash

INFILE="\$DATADIR/square.obj"
OUTFILE="\$TEMPDIR/$TEST.obj"
REFFILE="\$DATADIR/$REF"

\$BIN --$TEST $PARAM "\$INFILE" > "\$OUTFILE"

cmp -s "\$REFFILE" "\$OUTFILE"
exit \$?

EOF
