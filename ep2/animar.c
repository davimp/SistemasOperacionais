#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int n, d, i, j, t, a, b, c;
    FILE *f;

    f = fopen("corrida.dat", "r");

    fscanf(f, "%d %d", &n, &d);

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
                printf("%4d|", a);
            }
            printf("\n");
        }

        printf(" ");
        for(i = 0; i < d; i++)
            printf("_____");
        puts("");
        usleep(650000);
        system("clear");
    }

    fclose(f);



}
