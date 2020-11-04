#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
        "time_pista_antes.txt",
        "time_ciclistas_antes.txt",
        "memo_pista.txt",
        "memo_ciclistas.txt",
    };


for(k = 2; k < 4; k++)
{
    
    f = fopen(arq[k], "r");
    if(f == NULL)
    {
        printf("Não existe o arquivo %s\n", arq[k]);
        continue;
    }
    printf("%s:\n", arq[k]);

    //for(i = 1; i <= 30; i++)
    i = 1;
    while(fscanf(f, "%d %d", &tam, &i) != EOF)
    {
        if(k < 2)
        {
            fscanf(f, "%s %lf", s, cada+i); //real //https://edisciplinas.usp.br/mod/forum/discuss.php?d=609790
            fscanf(f, "%s %lf", s, &inutil); //usr
            fscanf(f, "%s %lf", s, &inutil); //sys
        }
        else //colocar informaçoẽs da memoria
        {
            fscanf(f, "%s %lfK", s, cada+i);
        }
        
        
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

            printf("   %s: %d ", k % 2 ?"nº de ciclistas":"tamanho da pista", tam);
            printf("média: %.2f   erro: %.2f\n", tempo, desvio);
        } 
    }

    fclose(f);

}
    return 0;
}
