#!/bin/sh

cd `dirname $0`

echo "build start"

SRC_DIR="../src/"
EXE_DIR="../exe/"
CPP_VERSION=17
CPP_COMMONS="${SRC_DIR}main.cpp ${SRC_DIR}SocketConnection.cpp"

NAME="LegacyServer"
echo "build ${NAME}..."
eval "clang++ -std=c++${CPP_VERSION} ${SRC_DIR}${NAME}.cpp -o ${EXE_DIR}${NAME}Cpp"

NAME="LegacyClient"
echo "build ${NAME}..."
eval "clang++ -std=c++${CPP_VERSION} ${SRC_DIR}${NAME}.cpp -o ${EXE_DIR}${NAME}Cpp"

NAME="SimpleServer"
echo "build ${NAME}..."
eval "clang++ -std=c++${CPP_VERSION} ${SRC_DIR}${NAME}.cpp ${CPP_COMMONS} -o ${EXE_DIR}${NAME}Cpp"

NAME="SimpleClient"
echo "build ${NAME}..."
eval "clang++ -std=c++${CPP_VERSION} ${SRC_DIR}${NAME}.cpp ${CPP_COMMONS} -o ${EXE_DIR}${NAME}Cpp"

NAME="Server"
echo "build ${NAME}..."
eval "mcs ${SRC_DIR}${NAME}.cs -out:${EXE_DIR}${NAME}Cs.exe"

NAME="Client"
echo "build ${NAME}..."
eval "mcs ${SRC_DIR}${NAME}.cs -out:${EXE_DIR}${NAME}Cs.exe"

echo "build finished"
