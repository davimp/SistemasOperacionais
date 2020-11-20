#include<iostream>
#include<string>
#ifndef ARQ
#define ARQ

typedef struct arquivo{
    /*int id;*/
    char* nome;
    int tamanho;
    int criado;
    int modificado;
    int acessado;
} Arquivo;

#endif