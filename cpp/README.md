Compile the C program:

```bash
g++ --std=c++11 -W -Wall -Wextra idriss-thread.cpp -lpthread -o idriss-thread
g++ --std=c++11 -W -Wall -Wextra idriss-nothread.cpp -o idriss
```

Run it (the first argument is `start`, the second is `max_bin_length`):

```bash
idriss-thread 3 10
idriss 3 10
```
