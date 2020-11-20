#include <iostream>
#include <fstream>

#define MAXN 1000000

using namespace std;

int bitmap[MAXN];
int FAT[MAXN];

int main(int argc, char *argv[]){

    int i,j;
    string nome_arq;
    string linha_lida, comando, argumentos[10];
    ofstream arq;

    //Shell
    while(1)
    {
        cout << "[ep3]: ";
        getline(cin, linha_lida);
        comando = linha_lida.substr(0,linha_lida.find(" "));
        j = comando.length();
        i = 0;
        //cout << linha_lida.
        while(j < linha_lida.length()){
            argumentos[i] = linha_lida.substr(j, linha_lida.find_first_not_of(" ", j));
            cout << linha_lida.find(" ", j) << "|"<< linha_lida[j] <<" ---> ";
            j += argumentos[i].length();
            cout << argumentos[i] << " " << j << endl;
            i++;
        }

        if(comando == "mount"){
            //argumento << linha_lida;
            cout << "monte " << argumentos[0] <<endl;
        }
        else if(comando == "ls"){
            cout << "liste " << argumentos[0] << endl;  
        }
        else if(comando == "mount"){

        }
        else{
            cout << "comando inválido!\n";
        }
    }

    return 0;
}