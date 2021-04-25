#!/bin/sh

cd `dirname $0`

if [ $# -lt 2 ]; then
    echo "usage: sh execute.sh <TYPE> <CLIENT/SERVER> <SCRIPT_ARGS...>"
    exit 1;
fi

EXE_DIR="../exe/"
TYPE=${1} # Simple, Legacy, Cs
SIDE=${2} # Client or Server
SCRIPT_ARG_SIZE=`expr $# - 2`
SCRIPT_ARGS=${SIDE}
if [ $SCRIPT_ARG_SIZE -ge 1 ]; then
    SCRIPT_ARGS="${SCRIPT_ARGS} ${3}"
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
    SCRIPT_NAME="SocketCs.exe"
    echo "mono \"${EXE_DIR}${SCRIPT_NAME} ${SCRIPT_ARGS}\""
    mono "${EXE_DIR}${SCRIPT_NAME} ${SCRIPT_ARGS}"
else
    SCRIPT_NAME="SocketCpp"
    echo "${EXE_DIR}${SCRIPT_NAME} ${TYPE} ${SCRIPT_ARGS}"
    ${EXE_DIR}${SCRIPT_NAME} ${TYPE} ${SCRIPT_ARGS}
fi
