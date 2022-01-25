Use the bash script to compile C++ the threaded program (with std::queue and raw circular buffer), the non threaded programs, run them, display the result of the algorithm and display the duration of algorithms. For example:

```bash
./test 1 30
```

Will return something like:
```bash
 ./test.sh 1 30
Compile threaded raw array
Compile threaded std::queue v0
Compile threaded std::queue v1
Compile no thread

=================
Run threaded std::queue v1
 Res: 44653447

real	0m5,359s
user	1m12,994s
sys	0m0,288s

=================
Run threaded raw array
Inputs: s=1, max=30:
 Res: 44653447

real	0m13,625s
user	0m51,152s
sys	0m0,736s

=================
Run threaded std::queue v0
Inputs: s=1, max=30:
Initial FIFO size: 17
 Res: 44653447

real	0m21,419s
user	0m56,560s
sys	0m0,084s

=================
Run no thread
Res: 44653447

real	0m44,537s
user	0m44,498s
sys	0m0,016s
```

Note: idriss-thread-queue-v1.cpp is the faster version.
