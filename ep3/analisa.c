#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXN 24

int main(){
    int i, j, k, tam;
    char num[10];
    char s[50], ss[50], sss[50], en[50];
    double cumpriu;
    double mudancas;
    double aux, inutil;
    double cada[50];
    double tempo ,desvio;
    FILE *f, *p;
    char *arq[] = {
        "res1.txt",
        "res2.txt",
        "res3.txt",
        "res4.txt",
        "res5.txt",
        "res6.txt",
        "res7.txt",
        "res8.txt",
        "res9.txt",
        "res10.txt",
        "res11.txt",
        "res12.txt",
        "res13.txt",
        "res14.txt",
        "res15.txt",
        "res16.txt",
        "res17.txt",
        "res18.txt",
        "res19.txt",
        "res20.txt",
        "res21.txt",
        "res22.txt",
        "res23.txt",
        "res24.txt",
    };


for(k = 0; k < MAXN; k++)
{
    
    f = fopen(arq[k], "r");
    if(f == NULL)
    {
        printf("Não existe o arquivo %s\n", arq[k]);
        continue;
    }
    printf("%s:\n", arq[k]);

    i = 1;
    while(fscanf(f, "%d", &i) != EOF)
    {

        fscanf(f, "%s %lf", s, cada+i); //real 
        fscanf(f, "%s %lf", s, &inutil); //usr
        fscanf(f, "%s %lf", s, &inutil); //sys
        
        if(i == 1)
        {
            tempo = 0.0;
        }

        tempo += cada[i];

        if(i == 30)
        {
            tempo /= 30;


            desvio = 0;
            for(j = 1; j <= 30; j++)
                desvio += (cada[i] - tempo)*(cada[i] - tempo);
            
            desvio /= 30*30;
            desvio = sqrt(desvio)*1.96;

            /*printf("   %s: %d ", k % 2 ?"nº de ciclistas":"tamanho da pista", tam);*/
            printf("média: %.2f   erro: %.2f\n", tempo, desvio);
        } 
    }

    fclose(f);

}
    return 0;
}
