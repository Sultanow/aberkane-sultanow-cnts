#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define T_SIZE ((uint64_t)100000000)
#define len(x) ((uint64_t)log2(x)+1)
#define typeB(x) ((x)%3==0)
#define typeA(x) ((x)%3==2)
#define typeC(x) ((x)%3==1)
#define Ag(x) (((x-17)%24)==0)
#define v(x) (4*(x)+1)

uint64_t t[T_SIZE];

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr,"Error, bad number of arguments on the command line\n");
        return 1;
    }

    // Inputs
    uint64_t s = atoi(argv[1]);
    uint64_t max = atoi(argv[2]); // max bin length
    //printf("Inputs: s=%zu, max=%zu\n", s, max);

    uint64_t X = 0;
    /*
      int *t=(int *) malloc(T_SIZE*4);
      if (t==NULL) {
      fprintf(stderr,"Pas assez de mémoire !!!\n");
      return 1;
      }
    */

    t[0] = s;
    uint64_t ls = (uint64_t)(ceil(log(s) / log(2)));
    uint64_t tContents=1, index=1; // index points to the first free space
    //printf("S=%zu bits long\n",len(s));

    uint64_t n;
    uint64_t i = 0;

    //printf("Contents = ");
    while (tContents > 0)
    {
        //printf("%zu, ",tContents);
        if (tContents >= T_SIZE)
        {
            //printf("\n***\n***\n*** On est au bout du tableau !\n***\n***\n***\n");
            exit(1);
        }
        while ((n = t[i]) == 0)
        {
            i = (i+1) % T_SIZE; // n = first positive number found
        }
        tContents--;
        t[i] = 0; // on supprime l'élément trouvé
        i = (i + 1) % T_SIZE;

        //printf("Contenu = %zu, i = %zu\n",tContents,i);
        //printf("N=%zu, bdig=%zu,", n, len(n));
        if (typeA(n))
        {
            //printf(" typeA: ");
            t[index] = (2 * n - 1) / 3;
            //printf("=> push: %zu ", t[index]);
            tContents++;
            index = (index + 1) % T_SIZE;
            if (Ag(n))
            {
                //printf("=> X=%zu ",X);
                X++;
            }
            if ((len(n)) < ls + max - 1)
            {
                t[index] = v(n);
                //printf("=> push: %zu", t[index]);
                tContents++;
                index = (index + 1) % T_SIZE;
            }
            //printf("\n");
        }
        else if (typeB(n))
        {
            //printf(" typeB: ");
            if (len(n) < ls + max - 1)
            {
                t[index] = v(n);
                //printf("=> push: %zu\n", t[index]);
                tContents++;
                index = (index + 1) % T_SIZE;
            }
        }
        else if (typeC(n))
        {
            //printf(" typeC: ");
            if ((len(n) < ls + max) && (n > 1))
            {
                t[index] = (4 * n - 1) / 3;
                //printf("=> push: %zu n", t[index]);
                tContents++;
                index = (index + 1) % T_SIZE;
            }
            if (len(n) < ls + max - 1)
            {
                t[index] = v(n);
                //printf("=> push: %zu ", t[index]);
                tContents++;
                index = (index + 1) % T_SIZE;
            }
            //printf("\n");
        }
    }
    //printf("\nRes: %zu\n",X);
}
