#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(){
    
    int i, j, k;
    int tf[1010], tr[1010], deadline[1010], aux_tf, aux_tr, aux_mud;
    char num[10];
    char fcfs[50], srtn[50], rr[50];
    char s[50], ss[50], sss[50], en[50];
    int trace[] = {1, 2, 5, 10, 20, 50, 100, 200};
    double cumpriu;
    double mudancas;
    FILE *f, *p;


    fcfs[0] = '\0';
    srtn[0] = '\0';
    rr[0] = '\0';

    strcat(fcfs, "saida_fcfs_trace");
    strcat(srtn, "saida_srtn_trace");
    strcat(rr, "saida_rr_trace");

    puts("FCFS:");
    for(j = 0; j < 1; j++)
    {
        sprintf(en, "trace%d", trace[j]);
        printf("%s\n", en);

        p = fopen(en, "r");

        k = 0;
        while(fscanf(p, "%s %d %d %d", ss, &aux_tf, &aux_tr, deadline + k) != EOF)
            k++;
        cumpriu = 0;


        for(i = 1; i <= 30; i++)
        {
            sprintf(s, "%s%d_%d", fcfs, trace[j], i);
            printf("%s\n", s);
            f = fopen(s, "r");
            k = 0;
            puts("logo");
            for(k = 0; k < trace[j]; k++)
            {
                fscanf(f, "%s %d %d", sss, &aux_tf, &aux_tr);

                if(aux_tf <= deadline[k])
                    cumpriu++;
                k++;
                puts("nem tao logo");
            }
            fscanf(f, "%%d", &aux_mud);

            mudancas += aux_mud;

            fclose(f);  
        }
        cumpriu /= 30;
        mudancas /= 30;

        printf("%.2f %.2f\n", cumpriu, mudancas);

        fclose(p);

    }
    
    puts("SRTN:");
    for(j = 0; j < 1; j++)
    {
        sprintf(en, "trace%d", trace[j]);
        printf("%s\n", en);

        p = fopen(en, "r");

        k = 0;
        while(fscanf(p, "%s %d %d %d", ss, &aux_tf, &aux_tr, deadline + k) != EOF)
            k++;
        cumpriu = 0;


        for(i = 1; i <= 30; i++)
        {
            sprintf(s, "%s%d_%d", srtn, trace[j], i);
            printf("%s\n", s);
            f = fopen(s, "r");
            k = 0;
            puts("logo");
            for(k = 0; k < trace[j]; k++)
            {
                fscanf(f, "%s %d %d", sss, &aux_tf, &aux_tr);

                if(aux_tf <= deadline[k])
                    cumpriu++;
                k++;
                puts("nem tao logo");
            }
            fscanf(f, "%%d", &aux_mud);

            mudancas += aux_mud;

            fclose(f);  
        }
        cumpriu /= 30;
        mudancas /= 30;

        printf("%.2f %.2f\n", cumpriu, mudancas);

        fclose(p);

    }
    
    puts("RR:");
    for(j = 0; j < 1; j++)
    {
        sprintf(en, "trace%d", trace[j]);
        printf("%s\n", en);

        p = fopen(en, "r");

        k = 0;
        while(fscanf(p, "%s %d %d %d", ss, &aux_tf, &aux_tr, deadline + k) != EOF)
            k++;
        cumpriu = 0;


        for(i = 1; i <= 30; i++)
        {
            sprintf(s, "%s%d_%d", rr, trace[j], i);
            printf("%s\n", s);
            f = fopen(s, "r");
            k = 0;
            puts("logo");
            for(k = 0; k < trace[j]; k++)
            {
                fscanf(f, "%s %d %d", sss, &aux_tf, &aux_tr);

                if(aux_tf <= deadline[k])
                    cumpriu++;
                k++;
                puts("nem tao logo");
            }
            fscanf(f, "%%d", &aux_mud);

            mudancas += aux_mud;

            fclose(f);  
        }
        cumpriu /= 30;
        mudancas /= 30;

        printf("%.2f %.2f\n", cumpriu, mudancas);

        fclose(p);

    }
    

    return 0;
}