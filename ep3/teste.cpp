#include <iostream>
#include <fstream>

#define MAXN 1000000

using namespace std;

int bitmap[MAXN];
int FAT[MAXN];

int main(int argc, char *argv[]){

    int i,j;
    string nome_arq, aux, conteudo;
    string linha_lida, comando, argumentos[10];
    fstream arq;

    cin >> aux;
    aux = aux.substr(aux.find_last_of("/")+1, aux.length());
    cout << aux << endl;

    cin >> aux;
    arq.open(aux, fstream::out | fstream::in);
    arq >> conteudo;
    cout << conteudo << endl;
}