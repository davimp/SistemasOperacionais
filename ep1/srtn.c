#include"srtn.h"

/* Função para as threads */
void * Thread(void * a) {
   long int count;
   time_t tempo_inicial, tempo_atual;
   int * arg = a;
   tempo_inicial = time(NULL);
   tempo_atual = time(NULL);
   count = -10000000;
   while((tempo_atual - tempo_inicial) < (*arg)){
      count++;
      tempo_atual = time(NULL);
   }
   return NULL;
}


/* EXPLICAÇÃO DA IDEIA
- duas filas: uma de prontos e uma de nao prontos
- a fila de prontos sera ordenada pelo dt
- struct para guardar as informacoes dos processos

    1) coloco tudo na fila de não prontos
    2) enquanto as duas filas não estiverem vazias eu
    faço o loop do 'clock':
        2.1) primeiro coloco na fila de prontos quem ja esta pronto em ordem de menor dt
        2.2) se a fila de prontos não estiver vazia, eu olho pro primeiro da fila
        e tento colocar ele na cpu. eu só coloco ele na cpu se o dt dele for menor do dt
        do processo que ja ta executando ou se a cpu estiver livre. caso contrario eu nao
        faço nada.
        3) se tiver algum processo executando na cpu diminuimos o dt dele no final do while
        (final do 'clock') e dou um sleep(1)

*/

void srtn(FILE* arq_trace, FILE* arq_saida)
{
    Processo pronto[10000], npronto[10000];
    int mutex;
    int i, j, k, n, tam_prontos, tam_nprontos;
    char nome[50]; /* nome do 'processo'*/
    int t0, dt, deadline; /* dados do 'processo'*/
    int tempo1, tempo2;
    int mudancas = 0;
    pthread_t tid[10000], atual;
    time_t tempo_inicial, tempo_atual, tempo_iniciou, tempo_acabou;
    int ok;
    tempo_inicial = time(NULL); /* começa a contar o tempo  */
    i=j=0;/* as filas estao vazias */
    n=k=0;
    /* Lê os processos e coloca no vetor de não prontos */
    while(fscanf(arq_trace, "%s %d %d %d", nome, &t0, &dt, &deadline) != EOF){
        npronto[n].nome = nome;
        npronto[n].t0 = t0;
        npronto[n].dt = dt;
        npronto[n].deadline = deadline;
        npronto[n].tempo_comecou = -1;
        npronto[n].tempo_acabou = -1;
        n++;
    }

    tempo_inicial = time(NULL);
    /* 
    Enquanto ainda tiver processos para executar, 
    ou seja, enquanto pelo menos uma das filas ainda
    nao estiver vazia   
    */
    while(i && (j != n)){
        tempo_atual = time(NULL);
        tempo1 = tempo_atual - tempo_inicial;
        /* quem esta pronto vai pra fila de prontos */
        for(k = j; k < n; k++){
            if(npronto[k].t0 >= tempo1){
                /*coloca na fila de prontos*/
                j++;
            }
        }

        /* se tem alguem pronto */
        if(i){
            if(mutex){ /* se a cpu está livre */
                /* coloca na cpu */
            }
            else{
                /* parte mais dificil
                - se tem alguem na cpu tem que comparar o dt dela com o 
                dt do primeiro da fila de prontos. se for vantajoso,
                a gente faz a troca. se não só continua. prestar atenção
                na troca.
                - usar semáforo?
                */
            }
        }
        sleep(1);
    }
    printf("%d\n", mudancas);
}
