#include <iostream>
#include <queue>
#include <cmath>
#include <limits>
#include <thread>
#include <future>
#include <condition_variable>
#include <mutex>
#include <sstream>

template <typename T>
class ThreadSafeQueue
{
public:

    bool pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (queue_.empty())
            return false;
        item = queue_.front();
        queue_.pop();
        return true;
    }

    void push(std::queue<T>& pushed_queue)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (!pushed_queue.empty())
        {
            queue_.push(std::move(pushed_queue.front()));
            pushed_queue.pop();
        }
    }

private:

    std::queue<T> queue_;
    std::mutex mutex_;
};

// Return the number of binary digits
static uint64_t length(uint64_t x)
{
    return uint64_t(log2(x) + 1.0);
}

static inline bool typeB(uint64_t x)
{
    return (x % 3u) == 0u;
}

static inline bool typeA(uint64_t x)
{
    return (x % 3u) == 2u;
}

static inline bool typeC(uint64_t x)
{
    return (x % 3u) == 1u;
}

static inline bool Ag(uint64_t x)
{
    return (((x - 17u) % 24u) == 0u);
}

static inline uint64_t V(uint64_t x)
{
    return 4u * x + 1u;
}

enum class FifoState
{
    Empty,
    NotEmpty
};
static FifoState process (std::queue<uint64_t> &fifo, uint64_t &cnt, uint64_t const ls, uint64_t const max)
{
    if (!fifo.empty())
    {
        auto const n = fifo.front();
        fifo.pop();

        uint64_t ln = length(n);
        uint64_t vn = V(n);

        if (typeA(n))
        {
            fifo.push((2u * n - 1u) / 3u);

            if (Ag(n))
            {
                cnt += 1u;
            }
            if (ln < ls + max - 1u)
            {
                fifo.push(vn);
            }
        }
        else if (typeB(n))
        {
            if (ln < ls + max - 1u)
            {
                fifo.push(vn);
            }
        }
        else if (typeC(n))
        {
            if ((ln < ls + max) && (n > 1u))
            {
                fifo.push((4u * n - 1u) / 3u);
            }
            if (ln < ls + max - 1u)
            {
                fifo.push(vn);
            }
        }
    }

    return fifo.empty() ? FifoState::Empty : FifoState::NotEmpty;
}

void threadFunc(ThreadSafeQueue<uint64_t> &tsfifo, std::atomic<uint64_t> &X, uint64_t const ls, uint64_t const max)
{
#define LOG_MAX 0

    uint64_t cntX = 0;
    std::queue<uint64_t> fifo;

#if LOG_MAX
    uint64_t max_n = 0u, max_size = 0u;
#endif


    while(true)
    {
        if (fifo.empty())
        {
            uint64_t val;
            if (!tsfifo.pop(val))
                break;

            fifo.push(val);
        }
        else
        {
            auto val = fifo.front();
            fifo.pop();
            tsfifo.push(fifo);
            fifo.push(val);
        }

        // Process the sequence until:
        // - there are no more values to process
        // - too much iterations have been performed
        // - the list of values has grown too large
        // The last 2 conditions are made to force a regular synchronization with
        // tsfifo and share the load between the threads.
        // The exact heuristics and values to be used are TBD.
        unsigned n = 0;
        while (process(fifo, cntX, ls, max) != FifoState::Empty && ++n < 10000000u && fifo.size() < 250000u)
        {
#if LOG_MAX
            max_size = (fifo.size() > max_size) ? fifo.size() : max_size;
#endif
        }

#if LOG_MAX
        max_n = (n > max_n) ? n : max_n;
#endif
    }

#if LOG_MAX
    std::stringstream sstr;
    sstr << "max n = " << max_n << ", max size = " << max_size << std::endl;
    std::cout << sstr.str();
#endif

    X += cntX;
}

int main(int argc, char *argv[])
{
    // Command line
    if (argc != 3)
    {
        std::cerr << "Error! Bad number of arguments:" << std::endl;
        std::cerr << "  " << argv[0] << " <s> <max_bin_len>" << std::endl;
        return EXIT_FAILURE;
    }

    // Inputs
    uint64_t const s = std::stoul(argv[1]);
    uint64_t const max = std::stoul(argv[2]); // max bin length
    uint64_t const ls = length(s);

    // Output (counter)
    std::atomic<uint64_t> X{0u};

    std::queue<uint64_t> fifo;
    fifo.push(s);

    auto const n_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    threads.reserve(n_threads);

    // Fill the fifo with enough elements to start n_threads threads
    uint64_t cnt = 0;
    FifoState fifoState;
    do
    {
        fifoState = process(fifo, cnt, ls, max);
    }
    while (fifoState != FifoState::Empty && fifo.size() < n_threads);

    X += cnt;

    // Start the threads
    ThreadSafeQueue<uint64_t> tsfifo;
    tsfifo.push(fifo);
    if (fifoState != FifoState::Empty)
    {
        for (unsigned i = 1; i < n_threads; ++i)
        {
            threads.emplace_back(&threadFunc, std::ref(tsfifo), std::ref(X), ls, max);
        }
    }
    threadFunc(tsfifo, X, ls, max);
    for (auto &t: threads)
        if (t.joinable())
            t.join();


    std::cout << " Res: " << X << std::endl;

    return EXIT_SUCCESS;
}
