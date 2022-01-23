from numba.cuda import target
import numpy as np
import pandas as pd
from numba import jit
from gmpy2 import mpz

import sys

@jit('void(uint64,uint64)')
def prod_sum(x: np.uint64, n: np.uint64) -> int:
    sum=0
    for l in range(1,(n-2)//x+1):
        prod=1
        for k in range(1,l+1):
            prod*=(n-x*k)//12
        sum+=prod
    print(sum)
    return sum

@jit('void(uint64,uint64)')
def prod_sum_gmpy2(x: np.uint64, n: np.uint64) -> int:
    sum=mpz(0)
    for l in np.arange(1, (n-2)//x+1, dtype=np.uint64):
        prod=mpz(1)
        for k in np.arange(1, l+1, dtype=np.uint64):
            prod=prod*mpz(n-x*k)//12
        sum+=prod
    print(n)
    return sum

def main() -> int:
    start = 3
    df = pd.read_csv('notebook/s_3_max_bin_length.csv')
    y_val=df['max_bin_length'].tolist()
    y_val3=[prod_sum_gmpy2(start,n) for n in y_val]
    with open("prod_sum.txt", "w") as output:
        output.write(str(y_val3))

    return 0

if __name__ == '__main__':
    sys.exit(main())
