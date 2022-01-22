#include <iostream>
#include <queue>
#include <cmath>
#include <limits>
#include <thread>
#include <future>
#include <condition_variable>
#include <mutex>

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

// https://juanchopanzacpp.wordpress.com/2013/02/26/concurrent-queue-c11/
template <typename T>
class ThreadSafeQueue
{
public:

    bool empty()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        return queue_.empty();
    }

    bool pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        if (queue_.empty())
            return false;
        item = queue_.front();
        queue_.pop();
        return true;
    }

    void push(const T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();
    }

private:

    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

// https://www.youtube.com/watch?v=w6dQQt10Dxo
class ThreadPool
{
public:

    ThreadPool()
    {
        int const thread_count = std::thread::hardware_concurrency();
        try
        {
            for (int i = 0; i < thread_count; ++i)
            {
                m_threads.push_back(std::thread(&ThreadPool::worker_thread, this));
            }
        }
        catch (std::exception const&)
        {
            m_done = true;
            throw;
        }
    }

    ~ThreadPool()
    {
        m_done = true;
        for (auto& it: m_threads)
        {
            if (it.joinable())
            {
                it.join();
            }
        }
    }

    template<typename Function>
    void submit(Function f)
    {
        m_work_queue.push(std::function<void()>(f));
    }

private:

    void worker_thread()
    {
        while (!m_done)
        {
            std::function<void()> task;
            if (m_work_queue.pop(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

private:

    std::atomic<bool> m_done{false};
    ThreadSafeQueue<std::function<void()>> m_work_queue;
    std::vector<std::thread> m_threads;
};

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
    uint64_t s = std::stoul(argv[1]);
    uint64_t max = std::stoul(argv[2]); // max bin length
    //std::cout << std::this_thread::get_id() << " Inputs: s=" << s << ", max=" << max << ":" << std::endl;

    // Output (counter)
    std::atomic<uint64_t> X{0u};

    ThreadSafeQueue<uint64_t> fifo;
    fifo.push(s);

    uint64_t ls = length(s);
    //std::cout << std::this_thread::get_id() << " S=" << s << ", bdig=" << ls << std::endl;

    { // Scope to force thread joins
        ThreadPool pool;
        for (int job = 0; job < 64; ++job)
        {
            pool.submit([&] {
                uint64_t n;
                while (fifo.pop(n))
                {
                    uint64_t ln = length(n);
                    uint64_t vn = V(n);

                    //printf("%lu: %zu\n", std::hash<std::thread::id>{}(std::this_thread::get_id()), n);
                    //std::cout << std::this_thread::get_id() << " N=" << n << ", bdig=" << ln << ", ";
                    if (typeA(n))
                    {
                        uint64_t tmp = (2u * n - 1u) / 3u;
                        //std::cout << std::this_thread::get_id() << " => push: " << tmp << " ";
                        fifo.push(tmp);

                        //std::cout << std::this_thread::get_id() << " typeA: ";
                        if (Ag(n))
                        {
                            X += 1u;
                            //std::cout << std::this_thread::get_id() << " => X=" << X  << " ";
                        }
                        if (ln < ls + max - 1u)
                        {
                            //if (n > std::numeric_limits<uint64_t>::max() / 2u)
                            //{
                            //    std::cerr << "Overflow 1" << std::endl;
                            //    return EXIT_FAILURE;
                            //}

                            //std::cout << std::this_thread::get_id() << " => push: " << vn << " ";
                            fifo.push(vn);
                        }
                        //std::cout << std::endl;
                    }
                    else if (typeB(n))
                    {
                        //std::cout << std::this_thread::get_id() << " typeB: ";
                        if (ln < ls + max - 1u)
                        {
                            //std::cout << std::this_thread::get_id() << " => push: " << vn << " ";
                            fifo.push(vn);
                        }
                        //std::cout << std::endl;
                    }
                    else if (typeC(n))
                    {
                        //std::cout << std::this_thread::get_id() << " typeC ";
                        if ((ln < ls + max) && (n > 1u))
                        {
                            //if (n > std::numeric_limits<uint64_t>::max() / 4u)
                            //{
                            //    std::cerr << "Overflow 2" << std::endl;
                            //    return EXIT_FAILURE;
                            //}

                            //std::cout << std::this_thread::get_id() << " => push: " << vn << " ";
                            fifo.push((4u * n - 1u) / 3u);
                        }
                        if (ln < ls + max - 1u)
                        {
                            //std::cout << std::this_thread::get_id() << " => push: " << vn << " ";
                            fifo.push(vn);
                        }
                        //std::cout << std::endl;
                    }
                }
            }); // thread func
        } // for
    }

    std::cout << " Res: " << X << std::endl;

    return EXIT_SUCCESS;
}
