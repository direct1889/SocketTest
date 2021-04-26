#!/bin/sh

cd `dirname $0`

echo "build start"

SRC_DIR="../src/"
EXE_DIR="../exe/"
CPP_VERSION=17
CPP_COMMONS="${SRC_DIR}main.cpp ${SRC_DIR}SocketConnection.cpp"
CS_COMMONS="${SRC_DIR}Main.cs ${SRC_DIR}SocketConnection.cs"

echo "build Cpp..."
eval "clang++ -std=c++${CPP_VERSION} ${SRC_DIR}LegacyClient.cpp ${SRC_DIR}LegacyServer.cpp ${CPP_COMMONS} -o ${EXE_DIR}SocketCpp"

echo "build Cs..."
eval "mcs ${SRC_DIR}Client.cs ${SRC_DIR}Server.cs ${SRC_DIR}Main.cs ${SRC_DIR}SocketConnection.cs -out:${EXE_DIR}SocketCs.exe"

echo "build finished"
