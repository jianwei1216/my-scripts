#!/bin/bash

# [exit.sh]

echo "hello"

./exit2.sh yes &

echo "world"

exit $?
