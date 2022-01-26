#!/bin/bash -e

#echo "Compile threaded raw array"
#g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-thread-raw.cpp -o idriss-threaded-raw -lpthread

#echo "Compile threaded std::queue v0"
#g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-thread-queue-v0.cpp -o idriss-threaded-queue-v0 -lpthread

echo "Compile threaded std::queue v1"
g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-thread-queue-v1.cpp -o idriss-threaded-queue-v1 -lpthread

echo "Compile no thread"
g++ --std=c++11 -W -Wall -Wextra -Wshadow-local idriss-nothread.cpp -o idriss-nothread

echo ""
echo "================="
echo "Run threaded std::queue v1"
time ./idriss-threaded-queue-v1 $1 $2

#echo ""
#echo "================="
#echo "Run threaded raw array"
#time ./idriss-threaded-raw $1 $2

#echo ""
#echo "================="
#echo "Run threaded std::queue v0"
#time ./idriss-threaded-queue-v0 $1 $2

echo ""
echo "================="
echo "Run no thread"
time ./idriss-nothread $1 $2
