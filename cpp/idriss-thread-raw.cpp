#include <iostream>
#include <queue>
#include <cmath>
#include <cstring>
#include <limits>
#include <vector>
#include <thread>

// ========================================================
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

// ========================================================
// Just a wrapper on FIFO: First In First Out (not thread safe)
#define FIFO_SIZE (32u * 1024u * 1024u)
class Fifo
{
public:

    Fifo(uint64_t block_size = FIFO_SIZE)
    {
       m_reserved = block_size;
       m_data = new uint64_t[m_reserved];
    }

    ~Fifo()
    {
       delete[] m_data;
    }

    bool pop(uint64_t& item)
    {
       if (m_contents == 0u)
        return false;

       --m_contents;
       while ((item = m_data[m_index_pop]) == 0)
       {
          m_index_pop = (m_index_pop + 1) % m_reserved;
       }
       m_data[m_index_pop] = 0;
       m_index_pop = (m_index_pop + 1) % m_reserved;
       return true;
    }

    void push(const uint64_t item)
    {
       ++m_contents;
       if (m_contents >= m_reserved)
       {
           printf("Memory full. Abort!\n");
           exit(1);
           //TODO
           //printf("Realloc\n");
           //uint64_t old_size = m_reserved;
           //m_reserved += FIFO_SIZE;
           //uint64_t* new_arr = new uint64_t[m_reserved];
           //memcpy(new_arr, m_data, old_size * sizeof(uint64_t));
           //delete[] m_data;
           //m_data = new_arr;
           //TODO: restore indices !!!
       }
       m_data[m_index_push] = item;
       m_index_push = (m_index_push + 1) % m_reserved;
    }

    inline uint64_t size() const
    {
        return m_contents;
    }

    inline uint64_t empty() const
    {
        return m_contents == 0u;
    }

private:

uint64_t* resize(uint64_t*& old, uint64_t& length, uint64_t const resize_to)
{
    length += resize_to;
    uint64_t* new_ptr = new uint64_t[length];
    uint64_t i = length;
    while (i--)
    {
        new_ptr[i] = old[i];
    }
    delete[] old;
    old = nullptr;
    return new_ptr;
}

private:

    uint64_t* m_data = nullptr;
    uint64_t m_index_push = 0;
    uint64_t m_index_pop = 0;
    uint64_t m_contents = 0;
    uint64_t m_reserved;
};

// ========================================================
// Number of workers = 2 * CPU cores (shall be >= 1)
#define JOBS 16

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
    uint64_t ls = length(s);

    std::cout << "Inputs: s=" << s << ", max=" << max << ":" << std::endl;

    // Transitioning elements for the algorithm
    Fifo fifos[JOBS]; // 1 FIFO for each thread

    // Output (counter)
    uint64_t X[JOBS]; // X for each threads

    for (int job = 0; job < JOBS; ++job)
    {
        X[job] = 0u;
    }

    // Main thread to fill the first results into the FIFO
    {
        Fifo fifo(JOBS * 2u);

        // Push the initial number
        fifo.push(s);

        uint64_t n;

        while (fifo.pop(n))
        {
            // Halt the main thread when enough data is reached for filling
            // other threads' FIFO (each thread fifo should have >= 1 element)
            if (fifo.size() > JOBS)
            {
                fifo.push(n);
                break;
            }

            uint64_t ln = length(n);
            uint64_t vn = V(n);

            if (typeA(n))
            {
                fifo.push((2u * n - 1u) / 3u);

                if (Ag(n))
                {
                    X[0] += 1u;
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

        // Transfer elements: FIFO => FIFOs
        uint64_t m;
        int job = 1;
        while (fifo.pop(m))
        {
            fifos[job].push(m);
            ++job;
            if (job == JOBS)
            {
                job = 0;
                continue;
            }
        }
    }

    // Threaded algorithm: 1 FIFO by thread
    {
        std::thread threads[JOBS];

        for (int job = 0; job < JOBS; ++job)
        {
            threads[job] = std::thread([&fifos, &X, &ls, &max, job] {
                uint64_t n;
                while (fifos[job].pop(n))
                {
                    uint64_t ln = length(n);
                    uint64_t vn = V(n);

                    if (typeA(n))
                    {
                        fifos[job].push((2u * n - 1u) / 3u);

                        if (Ag(n))
                        {
                            X[job] += 1u;
                        }
                        if (ln < ls + max - 1u)
                        {
                            fifos[job].push(vn);
                        }
                    }
                    else if (typeB(n))
                    {
                        if (ln < ls + max - 1u)
                        {
                            fifos[job].push(vn);
                        }
                    }
                    else if (typeC(n))
                    {
                        if ((ln < ls + max) && (n > 1u))
                        {
                            fifos[job].push((4u * n - 1u) / 3u);
                        }
                        if (ln < ls + max - 1u)
                        {
                            fifos[job].push(vn);
                        }
                    }
                }
            }); // end thread func
        } // end for

        // Wait for tasks ending
        for (int job = 0; job < JOBS; ++job)
        {
            if (threads[job].joinable())
            {
                threads[job].join();
            }
        }
    } // end scope

    // Summation on X
    {
        for (int job = 1; job < JOBS; ++job)
        {
            X[0] += X[job];
        }
    }

    std::cout << " Res: " << X[0] << std::endl;

    return EXIT_SUCCESS;
}
