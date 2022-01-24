#!/bin/bash

echo "Compile threaded"
g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-thread.cpp -o idriss-threaded -lpthread

echo "Compile no thread"
g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-nothread.cpp -o idriss-nothread

echo ""
echo "================="
echo "Run threaded"
time ./idriss-threaded $1 $2

echo ""
echo "================="
echo "Run no thread"
time ./idriss-nothread $1 $2
