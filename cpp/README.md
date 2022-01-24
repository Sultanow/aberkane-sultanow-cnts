Use the bash script to compile C++ the threaded program (with std::queue and raw circular buffer), the non threaded programs, run them, display the result of the algorithm and display the duration of algorithms. For example:

```bash
./test 3 10
```

Will return something like:
```bash
Compile threaded raw array
Compile threaded std::queue
Compile no thread

=================
Run threaded raw array
Inputs: s=1, max=27:
 Res: 5581975

real	0m1,500s
user	0m5,193s
sys	0m0,096s

=================
Run threaded std::queue
Inputs: s=1, max=27:
Initial FIFO size: 17
 Res: 5581975

real	0m2,651s
user	0m7,836s
sys	0m0,012s

=================
Run no thread
Res: 5581975

real	0m5,378s
user	0m5,374s
sys	0m0,004s
```

Note: std::queue is not good, it's memory is growly again and again (> 8GB).
