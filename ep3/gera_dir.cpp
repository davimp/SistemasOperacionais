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


    arq << "mount sistemadir2.txt" << endl;
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
    arq << "umount" << endl;
    arq << "sai" << endl;

    return 0;
}