#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#define MAXN 25000 /* quantidade de blocos */
#define BLOCO 4000 /* cada bloco pode ter até quatro mil caracteres */

using namespace std;

struct Arquivo{
    string nome;
    string diretorio;
    string conteudo;
    int inicio;
    int tamanho;
    int criado;
    int modificado;
    int acessado;
};

struct Diretorio{
    string nome;
    vector<int> arquivos;
    int criado;
    int modificado;
    int acessado;
};

int bitmap[MAXN];
int FAT[MAXN];
int num_diretorios, num_arquivos;
Diretorio diretorios[MAXN];
Arquivo arquivos[MAXN];
vector<int> grafo[MAXN];
unordered_map<string, int> d, a; /* liga o diretório ao seu id */

int proximo_livre(){
    int x;
    for(x = 39; x < MAXN; x++){
        if(!bitmap[x]){
            break;
        }
    }
    return x;
}

void imprime_arquivos(int id){
    for(int it : diretorios[id].arquivos){
        cout << arquivos[it].nome << endl;
    }
    for(int it : grafo[id]){
        imprime_arquivos(it);
    }    
}

int main(int argc, char *argv[]){
    int i,j,k;
    string nome_arq, s, conteudo, aux;
    string linha_lida, comando, argumentos[10];
    fstream arq, original;
    streampos pos;
    num_diretorios = num_arquivos = 0;
    d["/"] = 0;
    num_diretorios++;
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
            {   /* 125/4 25/4*/
                for(i = 0; i < (32 + 7)*BLOCO; i++)
                    arq << '0';
            }
            arq.seekg(0, fstream::beg);
        }
        else if(comando == "cp"){
            /*origem destino*/
            cin >> argumentos[0] >> argumentos[1];
            i = proximo_livre();
            
            /*crio o arquivo no bloco i*/
            if(i < MAXN)
            {
                /* pega o arquivo original */
                original.open(argumentos[0], fstream::in | fstream::out);

                aux = argumentos[1].substr(argumentos[1].find_last_of("/")+1, argumentos[1].length());
                a[argumentos[1]] = num_arquivos;
                argumentos[1] = argumentos[1].substr(0, argumentos[1].find_last_of("/")+1);
                j = d[argumentos[1]];
                (diretorios[j].arquivos).push_back(num_arquivos);

                /* cria o arquivo */
                arquivos[num_arquivos].nome = aux;
                original >> arquivos[num_arquivos].conteudo;
                arquivos[num_arquivos].tamanho = (arquivos[num_arquivos].conteudo).length();
                arquivos[num_arquivos].diretorio = argumentos[1];
                /* arquivos[num_arquivos].criado */
                /* arquivos[num_arquivos].modificado */
                /* arquivos[num_arquivos].acessado */
                num_arquivos++;
                original.close();
            }
            else
                cout << "Lotado" << endl;

            /* fecha original */
        }
        else if(comando == "mkdir"){
            /*diretorio*/
            cin >> argumentos[0];
            i = proximo_livre();

            /*crio o diretorio no bloco i*/
            if(i < MAXN)
            {
                d[argumentos[0]] = num_diretorios;
                aux = argumentos[0].substr(0, argumentos[0].find_last_of("/")+1);
                j = d[aux];
                grafo[j].push_back(num_diretorios);

                diretorios[num_diretorios].nome = argumentos[0];
                /*diretorios[num_diretorios].criado*/
                /*diretorios[num_diretorios].modificado*/
                /*diretorios[num_diretorios].acessado*/
                d[argumentos[0]] = num_diretorios;
                num_diretorios++;
            }
            else
                cout << "Lotado" << endl;

        }
        else if(comando == "rmdir"){
            /*diretorio*/
            cin >> argumentos[0];
        }
        else if(comando == "cat"){
            /* arquivo */
            cin >> argumentos[0];
            j = a[argumentos[0]];
            cout << arquivos[j].conteudo << endl;
        }
        else if(comando == "touch"){
            /* arquivo */
            cin >> argumentos[0];
        }
        else if(comando == "ls"){
            /* diretorio */
            cin >> argumentos[0];
            j = d[argumentos[0]];
            imprime_arquivos(j);
        }
        else if(comando == "find"){
            /*diretorio arquivo*/
            cin >> argumentos[0] >> argumentos[1];
        }
        else if(comando == "df"){
        }
        else if(comando == "umount"){
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