#!/bin/bash -e

echo "Compile threaded raw array"
g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-thread-raw.cpp -o idriss-threaded-raw -lpthread

echo "Compile threaded std::queue"
g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-thread-queue.cpp -o idriss-threaded-queue -lpthread

echo "Compile no thread"
g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-nothread.cpp -o idriss-nothread

echo ""
echo "================="
echo "Run threaded raw array"
time ./idriss-threaded-raw $1 $2

echo ""
echo "================="
echo "Run threaded std::queue"
time ./idriss-threaded-queue $1 $2

echo ""
echo "================="
echo "Run no thread"
time ./idriss-nothread $1 $2
