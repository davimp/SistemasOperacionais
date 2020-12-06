#include<bits/stdc++.h>
using namespace std;

#define QUANTOS 50

int main(){
    fstream arq;
    string s,t;
    char aux;
    int tam;
    cout << "Digite o nome do arquivo:";
    cin >> s;
    remove(s.data());
    arq.open(s, fstream::out | fstream::in | fstream::app);


    arq << "mount sistemadir6.txt" << endl;

    s = "/dir1";
    arq << "mkdir " << s << endl;
    for(int i = 2; i <= 31; i++){
        s += "/dir" + to_string(i);
        arq << "mkdir " << s << endl;
    }
    
    s = "/dir1";
    for(int j = 0; j < QUANTOS; j++){
        arq << "cp medio.txt " << s << "/teste" << j << ".txt" << endl;
    }
    for(int i = 2; i <= 31; i++){
        s += "/dir" + to_string(i);
        for(int j = 0; j < QUANTOS; j++){
            arq << "cp medio.txt " << s << "/teste" << j << ".txt" << endl;
        }
    }
    tam = 6508000;
    int j = 0;
    while(tam < 10000000){
        arq << "cp novo.txt " << "/teste" << j << ".txt" << endl;
        j++;
        tam += 520000;
    }
    j = 0;
    arq << "mkdir /segundo" << endl;
    while(tam < 20000000){
        arq << "cp novo.txt " << "/segundo/teste" << j << ".txt" << endl;
        j++;
        tam += 520000;
    }
    j = 0;
    arq << "mkdir /tercerio" << endl;
    while(tam < 30000000){
        arq << "cp novo.txt " << "/terceiro/teste" << j << ".txt" << endl;
        j++;
        tam += 520000;
    }
    j = 0;
    arq << "mkdir /quarto" << endl;
    while(tam < 40000000){
        arq << "cp novo.txt " << "/quarto/teste" << j << ".txt" << endl;
        j++;
        tam += 520000;
    }
    j = 0;
    arq << "mkdir /quinto" << endl;
    while(tam < 50000000){
        arq << "cp novo.txt " << "/quinto/teste" << j << ".txt" << endl;
        j++;
        tam += 520000;
    }

    arq << "umount" << endl;
    arq << "sai" << endl;

    return 0;
}