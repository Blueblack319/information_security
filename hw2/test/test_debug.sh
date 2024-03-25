#!/bin/bash

set +x
set +e

echo "checking for README"
if [ ! -e "./README" ]
then
        echo "Error: No README file"
        exit 1
fi

echo "checking for Makefile"
if [ ! -e "./Makefile" ]
then
        echo "Error: No Makefile file"
        exit 1
fi

echo "Running make"
make
rc=$?
if [ $rc -ne 0 ]
then
        echo "Error when running the make command"
        exit 1
fi

echo "Running make again"
make
rc=$?
if [ $rc -ne 0 ]
then
        echo "Error when running the make command again"
        exit 1
fi

if [ ! -e "./secure_house" ]
then
        echo "Error: Running make did not create the secure_house file"
        exit 1
fi

if [ ! -x "./secure_house" ]
then
        echo "Error: secure_house is not executable"
        exit 1
fi

INPUT_CASE="TURN KEY jh
WHO'S INSIDE?
INSERT KEY jh k3
TURN KEY jh
ENTER HOUSE david
INSERT KEY jh k1
TURN KEY jh
ENTER HOUSE jh
INSERT KEY david k1
TURN KEY david
ENTER HOUSE david
INSERT KEY hera k1
TURN KEY hera
ENTER HOUSE hera
WHO'S INSIDE?
LEAVE HOUSE david
WHO'S INSIDE?
LEAVE HOUSE ostin
CHANGE LOCKS hera K KK KKK
CHANGE LOCKS ostin K KK KKK
CHANGE LOCKS jh K KK KKK

"

CORRECT_OUTPUT="FAILURE jh HAD NO KEY INSERTED
NOBODY HOME
KEY k3 INSERTED BY jh
FAILURE jh HAD INVALID KEY k3 INSERTED
ACCESS DENIED
KEY k1 INSERTED BY jh
SUCCESS jh TURNS KEY k1
ACCESS ALLOWED
KEY k1 INSERTED BY david
SUCCESS david TURNS KEY k1
ACCESS ALLOWED
KEY k1 INSERTED BY hera
SUCCESS hera TURNS KEY k1
ACCESS ALLOWED
jh, david, hera
david LEFT
jh, hera
ostin NOT HERE
LOCK CHANGE DENIED
LOCK CHANGE DENIED
LOCK CHANGED
"

echo "Testing your program"
OUTPUT=$( echo -n "$INPUT_CASE" | ./secure_house jh k1 k2)

echo "Your program's output is as follows:"
echo "------------------------------------"
echo "$OUTPUT"
echo "------------------------------------"

DIFF=$(diff -aBw <(echo "$OUTPUT") <(echo "$CORRECT_OUTPUT"))
rc=$?
if [ $rc -ne 0 ]
then
	echo "Error: did not pass the basic test case on the website."
	echo "$DIFF"
else
	echo "SUCCESS!"
fi
