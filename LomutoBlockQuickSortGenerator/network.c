/* Calling bose(n) generates a network
 * to sort n items. See R. C. Bose & R. J. Nelson,
 * "A Sorting Problem", JACM Vol. 9, Pp. 282-296. */
#include <stdio.h>

P(int i, int j)
{
    /* print out in 0 based notation */
    printf("swap(%d, %d);\n", i-1, j-1);
}

void Pbracket(int i,  /* value of first element in sequence 1 */
         int x,  /* length of sequence 1 */
         int j,  /* value of first element in sequence 2 */
         int y)  /* length of sequence 2 */
{
    int a, b;

    if(x == 1 && y == 1)
        P(i, j); /* 1 comparison sorts 2 items */
    else if(x == 1 && y == 2)
    {
        /* 2 comparisons inserts an item into an
         * already sorted sequence of length 2. */
        P(i, (j + 1));
        P(i, j);
    }
    else if(x == 2 && y == 1)
    {
        /* As above, but inserting j */
        P(i, j);
        P((i + 1), j);
    }
    else
    {
        /* Recurse on shorter sequences, attempting
         * to make the length of one subsequence odd
         * and the length of the other even. If we
         * can do this, we eventually merge the two. */
        a = x/2;
        b = (x & 1) ? (y/2) : ((y + 1)/2);
        Pbracket(i, a, j, b);
        Pbracket((i + a), (x - a), (j + b), (y - b));
        Pbracket((i + a), (x - a), j, b);
    }
}

void Pstar(int i, /* value of first element in sequence */
      int m) /* length of sequence */
{
    int a;

    if(m > 1)
    {
        /* Partition into 2 shorter sequences,
         * generate a sorting method for each,
         * and merge the two sub-networks. */
        a = m/2;
        Pstar(i, a);
        Pstar((i + a), (m - a));
        Pbracket(i, a, (i + a), (m - a));
    }
}

void bose(int n)
{
    Pstar(1, n); /* sort the sequence {X1,...,Xn} */
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage:\n");
        printf("\t bose-nelson <n>\n");
        printf("\t\twhere n is the number of items to sort\n");
        return(-1);
    }
    int n = atoi(argv[1]);
    bose(n);
    return 0;
}

/* End of File */