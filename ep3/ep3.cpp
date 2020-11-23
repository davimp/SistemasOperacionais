#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#define MAXN 25000 /* quantidade de blocos */
#define BLOCO 4000 /* cada bloco pode ter até quatro mil caracteres */

using namespace std;

struct Arquivo{
    string nome;
    int tamanho;
    int criado;
    int modificado;
    int acessado;
};

struct Diretorio{
    string nome;
    int tamanho;
    int criado;
    int modificado;
    int acessado;
} ;

int bitmap[MAXN];
int FAT[MAXN];
vector<Diretorio> grafo[MAXN]; //mkdir /a/b/c
map<string, Diretorio> dir[MAXN];

int main(int argc, char *argv[]){
    int i,j;
    string nome_arq, s;
    string linha_lida, comando, argumentos[10];
    fstream arq;
    streampos pos;

    /* espaço ocupado pelo fat e pelo bitmap no início do arquivo */
    for(i = 0; i < 39; i++){
        bitmap[i] = 1;
    }

    /*""shell""*/
    while(1)
    {
        cout << "[ep3]: ";
        cin >> comando;

        if(comando == "mount"){
            /* parser */
            /* arquivo */
            cin >> argumentos[0];
            cout << "monte " << argumentos[0] << endl;
            arq.open(argumentos[0], fstream::out | fstream::in | fstream::app);
            arq.seekg(0, fstream::beg);
            pos = arq.tellg();

            arq >> s;
            cout << s << endl;

            if(arq.eof())
            {   /*125/4 25/4*/
                for(i = 0; i < (32 + 7)*BLOCO; i++)
                    arq << '0';
            }
            arq.seekg(0, fstream::beg);
        }
        else if(comando == "cp"){
            /*origem destino*/
            cin >> argumentos[0] >> argumentos[1];
        }
        else if(comando == "mkdir"){
            /*diretorio*/
            cin >> argumentos[0];


            for(i = 39; i < 25000; i++){
                if(!bitmap[i]){
                    break;
                }
            }


            /*crio o diretorio no bloco i*/
            if(i < 25000)
            {
                
            }
            else
                cout << "Lotado" << endl;

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
            arq.close();
            /*writer*/
        }
        else if(comando == "sai"){
            break;
        }
        else{
            cout << "comando inválido!\n";
        }
    }

    return 0;
}