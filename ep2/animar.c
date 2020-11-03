#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int n, d, i, j, t, a, b, c;
    FILE *f;

    f = fopen("corrida.dat", "r");

    fscanf(f, "%d %d", &d, &n);
    //printf("%d %d\n", d, n);
    //return 0;

    while(fscanf(f, "%d", &t) !=  EOF)
    {
        printf("tempo: %d\n", t);

        printf(" ");
        for(i = 0; i < d; i++)
            printf("_____");
        puts("");

        for(j = 0; j < 10; j++)
        {
            printf("|");
            for(i = 0; i < d; i++)
            {
                fscanf(f, "%d", &a);
                if(a != 0)
                    printf("\033[0;31m%4d\033[0m|", a);
                else
                    printf("   0|");
            }
            printf("\n");
        }

        printf(" ");
        for(i = 0; i < d; i++)
            printf("_____");
        puts("");
        usleep(500000);
        //system("clear");
    }

    fclose(f);



}
