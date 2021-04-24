#!/bin/sh

cd `dirname $0`

if [ $# -lt 2 ]; then
    echo "usage: sh execute.sh <TYPE> <CLIENT/SERVER> <SCRIPT_ARGS...>"
    exit 1;
fi

EXE_DIR="../exe/"
TYPE=${1}
SIDE=${2}
SCRIPT_ARG_SIZE=`expr $# - 2`
SCRIPT_ARGS=""
if [ $SCRIPT_ARG_SIZE -ge 1 ]; then
    SCRIPT_ARGS=" ${3}"
    if [ $SCRIPT_ARG_SIZE -ge 2 ]; then
        SCRIPT_ARGS="${SCRIPT_ARGS} ${4}"
        echo "script args: 2 {${SCRIPT_ARGS} }"
    else
        echo "script args: 1 {${SCRIPT_ARGS} }"
    fi
else
    echo "script args: none"
fi

echo "type: ${TYPE}, side: ${SIDE}"
if [ $TYPE == "Cs" ]; then
    SCRIPT_NAME="${SIDE}Cs.exe"
    mono "${EXE_DIR}${SCRIPT_NAME}"
else
    SCRIPT_NAME="${TYPE}${SIDE}Cpp"
    ${EXE_DIR}${SCRIPT_NAME}${SCRIPT_ARGS}
fi
