#include<bits/stdc++.h>
using namespace std;



int main(){
    fstream arq;
    string s;
    char aux;
    int tam;
    aux = 'a';
    cout << "Digite o nome do arquivo a ser criado: ";
    cin >> s;
    cout << "Digite o tamanho do arquivo em bytes: ";
    cin >> tam;
    remove(s.data());
    arq.open(s, fstream::out | fstream::in | fstream::app);
    arq << "mount sistemadir2.txt\n";
    
    for(int i = 0; i < tam; i++){
        arq << 't';
    }

    return 0;
}