#include <iostream>
#include <queue>
#include <cmath>
#include <limits>

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
    //std::cout << "Inputs: s=" << s << ", max=" << max << ":" << std::endl;

    // Output (counter)
    uint64_t X = 0u;

    std::queue<uint64_t> fifo;
    fifo.push(s);

    uint64_t ls = length(s);
    //std::cout << "S=" << s << ", bdig=" << ls << std::endl;

    while (!fifo.empty())
    {
        uint64_t n = fifo.front(); fifo.pop();
        uint64_t ln = length(n);
        uint64_t vn = V(n);

        //std::cout << "N=" << n << ", bdig=" << ln << ", ";
        if (typeA(n))
        {
            //std::cout << "typeA: ";
            uint64_t tmp = (2u * n - 1u) / 3u;
            //std::cout << "=> push: " << tmp << " ";
            fifo.push(tmp);

            if (Ag(n))
            {
                X += 1u;
                //std::cout << "=> X=" << X  << " ";
            }
            if (ln < ls + max - 1u)
            {
                if (n > std::numeric_limits<uint64_t>::max() / 2u)
                {
                    std::cerr << "Overflow 1" << std::endl;
                    return EXIT_FAILURE;
                }

                //std::cout << "=> push: " << vn << " ";
                fifo.push(vn);
            }
            //std::cout << std::endl;
        }
        else if (typeB(n))
        {
            //std::cout << "typeB: ";
            if (ln < ls + max - 1u)
            {
                //std::cout << "=> push: " << vn << " ";
                fifo.push(vn);
            }
            //std::cout << std::endl;
        }
        else if (typeC(n))
        {
            //std::cout << "typeC ";
            if ((ln < ls + max) && (n > 1u))
            {
                if (n > std::numeric_limits<uint64_t>::max() / 4u)
                {
                    std::cerr << "Overflow 2" << std::endl;
                    return EXIT_FAILURE;
                }

                //std::cout << "=> push: " << vn << " ";
                fifo.push((4u * n - 1u) / 3u);
            }
            if (ln < ls + max - 1u)
            {
                //std::cout << "=> push: " << vn << " ";
                fifo.push(vn);
            }
            //std::cout << std::endl;
        }
    }

    std::cout << "Res: " << X << std::endl;

    return EXIT_SUCCESS;
}
