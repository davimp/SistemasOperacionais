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
        /*getline(cin, linha_lida);*/
        /*comando = linha_lida.substr(0,linha_lida.find(" "));*/
        cin >> comando;

        if(comando == "mount"){
            /* arquivo */
            cin >> argumentos[0];
            cout << "monte " << argumentos[0] << endl;
        }
        else if(comando == "cp"){
            /*origem destino*/
            cin >> argumentos[0] >> argumentos[1];
        }
        else if(comando == "mkdir"){
            /*diretorio*/
            cin >> argumentos[0];
        }
        else if(comando == "rmdir"){
            /*diretorio*/
            cin >> argumentos[0];
        }
        else if(comando == "cat"){
            /*arquivo*/
            cin >> argumentos[0];
        }
        else if(comando == "touch"){
            /*arquivo*/
            cin >> argumentos[0];
        }
        else if(comando == "ls"){
            /*arquivo*/
            cin >> argumentos[0];
        }
        else if(comando == "find"){
            /*diretorio arquivo*/
            cin >> argumentos[0] >> argumentos[1];
        }
        else if(comando == "df"){
        }
        else if(comando == "unmount"){
        }
        else if(comando == "sai"){
        }
        else{
            cout << "comando inválido!\n";
        }
    }

    return 0;
}