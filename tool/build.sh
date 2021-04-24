#!/bin/sh

cd `dirname $0`

echo "build start"

NAME="LegacyServer"
echo "build ${NAME}..."
clang++ -std=c++17 "../src/${NAME}.cpp" -o "../exe/${NAME}Cpp"

NAME="LegacyClient"
echo "build ${NAME}..."
clang++ -std=c++17 "../src/${NAME}.cpp" -o "../exe/${NAME}Cpp"

NAME="SimpleServer"
echo "build ${NAME}..."
clang++ -std=c++17 "../src/${NAME}.cpp" -o "../exe/${NAME}Cpp"

NAME="SimpleClient"
echo "build ${NAME}..."
clang++ -std=c++17 "../src/${NAME}.cpp" -o "../exe/${NAME}Cpp"

NAME="Server"
echo "build ${NAME}..."
mcs "../src/${NAME}.cs" "-out:../exe/${NAME}Cs.exe"

NAME="Client"
echo "build ${NAME}..."
mcs "../src/${NAME}.cs" "-out:../exe/${NAME}Cs.exe"

echo "build finished"
