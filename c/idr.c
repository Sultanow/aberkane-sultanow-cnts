#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define T_SIZE 100000000
#define len(x) ((unsigned int)log2(x)+1)
#define typeB(x) ((x)%3==0)
#define typeA(x) ((x)%3==2)
#define typeC(x) ((x)%3==1)
#define Ag(x) (((x-17)%24)==0)
#define v(x) (4*x+1)

unsigned int t[T_SIZE];

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr,"Error, bad number of arguments on the command line\n");
        return 1;
    }

    // Inputs
    int s = atoi(argv[1]);
    int max = atoi(argv[2]); // max bin length
    printf("Inputs: s=%d, max=%d\n",s,max);

    unsigned int X = 0;
    /*
      int *t=(int *) malloc(T_SIZE*4);
      if (t==NULL) {
      fprintf(stderr,"Pas assez de mémoire !!!\n");
      return 1;
      }
    */

    t[0] = s;
    unsigned int ls = (unsigned int)(ceil(log(s) / log(2)));
    unsigned int tContents=1, index=1; // index points to the first free space
    //printf("S=%d bits long\n",len(s));

    unsigned int n;
    unsigned int i = 0;

    //printf("Contents = ");
    while (tContents > 0)
    {
        //printf("%d, ",tContents);
        if (tContents >= T_SIZE)
        {
            printf("\n***\n***\n*** On est au bout du tableau !\n***\n***\n***\n");
            exit(1);
        }
        while ((n = t[i]) == 0)
        {
            i = (i+1) % T_SIZE; // n = first positive number found
        }
        tContents--;
        t[i] = 0; // on supprime l'élément trouvé
        i = (i + 1) % T_SIZE;

        //printf("Contenu = %d, i = %d\n",tContents,i);
        //printf("N=%d, bin digits=%d,",n,len(n));
        if (typeA(n))
        {
            //printf("typeA: ");
            t[index] = (2 * n - 1) / 3;
            tContents++;
            index = (index + 1) % T_SIZE;
            //printf("=> push: %d ",t[index-1]);
            if (Ag(n))
            {
                //printf("=> X=%d ",X);
                X++;
            }
            if ((len(n)) < ls + max - 1)
            {
                t[index] = v(n);
                tContents++;
                index = (index + 1) % T_SIZE;
                //printf("=> push: %d\n",t[index-1]);
            }
        }
        else if (typeB(n))
        {
            //printf("typeB: ");
            if (len(n) < ls + max - 1)
            {
                t[index] = v(n);
                tContents++;
                index = (index + 1) % T_SIZE;
                //printf("=> push: %d\n",t[index-1]);
            }
        }
        else if (typeC(n))
        {
            //printf("typeC: ");
            if ((len(n) < ls + max) && (n > 1))
            {
                t[index] = (4 * n - 1) / 3;
                tContents++;
                //printf("=> push: %d ",t[index-1]);
                index = (index + 1) % T_SIZE;
            }
            if (len(n) < ls + max - 1)
            {
                t[index] = v(n);
                tContents++;
                //printf("=> push: %d",t[index-1]);
                index = (index + 1) % T_SIZE;
            }
            //printf("\n");
        }
    }
    printf("\nRes: %d\n",X);
}
