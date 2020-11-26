#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <time.h>
#define MAXN 25000 /* quantidade de blocos */
#define BLOCO 4000 /* cada bloco pode ter até quatro mil caracteres */

using namespace std;

struct Arquivo{
    string nome;
    string diretorio;
    string conteudo;
    int inicio;
    int tamanho;
    time_t criado;
    time_t modificado;
    time_t acessado;
};

struct Diretorio{
    string nome;
    vector<int> arquivos;
    int local;
    time_t criado;
    time_t modificado;
    time_t acessado;
};

int bitmap[MAXN];
int FAT[MAXN];
int livre;
int num_diretorios, num_arquivos;
Diretorio diretorios[MAXN];
//unordered_map<int, Diretorio> diretorios;
Arquivo arquivos[MAXN];
//unordered_map<int, Arquivo> arquivos;
vector<int> grafo[MAXN];
unordered_map<string, int> d, a; /* liga o caminho do diretório/arquivo ao seu id */


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
    cout << diretorios[id].nome << endl;
    for(int it : diretorios[id].arquivos){
        cout << arquivos[it].nome << endl;
    }
    for(int it : grafo[id]){
        imprime_arquivos(it);
    }
}

void find(int id, string nome, string caminho){

    caminho += diretorios[id].nome + "/";

    for(int it : diretorios[id].arquivos){
        if(arquivos[it].nome == nome) cout << caminho << arquivos[it].nome << endl;
    }
    for(int it : grafo[id]){
        find(it, nome, caminho);
    }
}

void rm(string n){
    string aux;
    int i,j,k;

    /* pegas os ids */
    aux = n.substr(0, n.find_last_of("/"));
    i = a[n];
    a[n] = 0;
    j = d[aux];

    /* apaga arquivo no diretório */
    for(auto it = (diretorios[j].arquivos).begin(); it < (diretorios[j].arquivos).end(); it++){
        if(*it == i){
            (diretorios[j].arquivos).erase(it);
            break;
        }
    }

    /* atualiza bitmap e FAT */
    k = arquivos[i].inicio;
    while(FAT[k] != -1){
        bitmap[k] = 0;
        j = FAT[k];
        FAT[k] = -1;
        k = j;
        livre++;
    }
    bitmap[k] = 0;
    livre++;

    /* apaga arquivo na lista de arquivos */
    arquivos[i].nome = "";
    arquivos[i].diretorio = "";
    arquivos[i].conteudo = "";
    arquivos[i].inicio = -1;
    arquivos[i].tamanho = -1;
    arquivos[i].criado = -1;
    arquivos[i].modificado = -1;
    arquivos[i].acessado = -1;
}

void rmdir(int id){
    string aux, n;
    int i,j,k;

    for(int it : grafo[id]){
        rmdir(it);
    }

    /* apaga diretório no grafo */
    for(auto it = (grafo[j]).begin(); it < (grafo[j]).end(); it++){
        if(*it == id){
            (grafo[j]).erase(it);
            break;
        }
    }

    /* apaga arquivos que estavam no diretorio */
    for(int i : diretorios[id].arquivos){
        rm(n + "/" + arquivos[i].nome);
    }

    /* atualiza bitmap e FAT */
    bitmap[diretorios[id].local] = 0;

    /* apaga diretório na lista de arquivos */
    diretorios[id].nome = "";
    diretorios[id].arquivos.clear();
    diretorios[id].local = -1;
    diretorios[id].criado = -1;
    diretorios[id].modificado = -1;
    diretorios[id].acessado = -1;
}

int main(int argc, char *argv[]){
    int i,j,k, next_id, blocos, cont;
    string nome_arq, s, content, aux;
    string linha_lida, comando, argumentos[10];
    fstream arq, original;
    streampos pos;
    num_diretorios = num_arquivos = 0;
    livre = 25000; /* quantos blocos estão livres */

    /* inicia os vetores de diretorios e arquivos */
    for(i = 0; i < MAXN; i++){
        diretorios[i].nome = "";
        diretorios[i].arquivos.clear();
        diretorios[i].local = -1;
        diretorios[i].criado = -1;
        diretorios[i].modificado = -1;
        diretorios[i].acessado = -1;
    }
    for(i = 0; i < MAXN; i++){
        arquivos[i].nome = "";
        arquivos[i].diretorio = "";
        arquivos[i].conteudo = "";
        arquivos[i].inicio = -1;
        arquivos[i].tamanho = -1;
        arquivos[i].criado = -1;
        arquivos[i].modificado = -1;
        arquivos[i].acessado = -1;
    }

    /* inicia o primeiro diretorio */
    d["/"] = 0;
    diretorios[0].nome = "/";
    diretorios[0].local = 40;
    diretorios[0].acessado = time(NULL);
    diretorios[0].criado = time(NULL);
    diretorios[0].modificado = time(NULL);
    num_diretorios++;

    /* espaço ocupado por fat + bitmap + primeiro diretorio no início do arquivo */

    for(i = 0; i < 40; i++){
        bitmap[i] = 1;
    }
    livre -= 40;

    /* shell */
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

            arq >> s;
            cout << s << endl;

            if(arq.eof())
            {   /* 125/4 25/4*/
                for(i = 0; i < (32 + 7)*BLOCO; i++)
                    arq << '0';
            }
        }
        else if(comando == "cp"){
            /*origem destino*/
            cin >> argumentos[0] >> argumentos[1];

            /* pega o conteudo do arquivo original */
            original.open(argumentos[0], fstream::in | fstream::out);
            original >> content;
            blocos = content.length();
            blocos = (blocos + 4000 - 1)/4000;
            /* fecha original */
            original.close();

            /*crio o arquivo no bloco i se tiver espaço para ele*/
            if(blocos <= livre)
            {
                /* acha o qual será o id do arquivo */
                for(i = 0; i < MAXN; i++){
                    if(arquivos[i].nome == "") break;
                }
                next_id = i;

                /* ajusa bitmap e FAT*/
                livre -= blocos;
                j = -1;
                while(blocos--){
                    i = proximo_livre();
                    bitmap[i] = 1;
                    if(j != -1) FAT[j] = i;
                    else{
                        arquivos[next_id].inicio = i;
                    }
                    FAT[i] = -1;
                    j = i;
                }

                /* pega os nomes e ajusta os unordered maps */
                aux = argumentos[1].substr(argumentos[1].find_last_of("/")+1, argumentos[1].length());
                a[argumentos[1]] = next_id;
                argumentos[1] = argumentos[1].substr(0, argumentos[1].find_last_of("/"));
                j = d[argumentos[1]];
                (diretorios[j].arquivos).push_back(next_id);

                /* cria o arquivo */
                arquivos[next_id].nome = aux;
                arquivos[next_id].conteudo = content;
                arquivos[next_id].tamanho = (content).length();
                arquivos[next_id].diretorio = argumentos[1];
                arquivos[next_id].criado = time(NULL);
                arquivos[next_id].modificado = time(NULL);
                arquivos[next_id].acessado =  time(NULL);
                num_arquivos++;
            }
            else
                cout << "Não há espaço" << endl;

        }
        else if(comando == "mkdir"){
            /*diretorio*/
            cin >> argumentos[0];

            if(d.count(argumentos[0])) /* caso já tenha sido criado, continua */
                continue;

            i = proximo_livre(); /* diretórios ocuparão um bloco apenas */

            /* crio o diretorio no bloco i */
            if(i < MAXN)
            {
                /* acha qual o id do diretorio */
                for(i = 0; i < MAXN; i++){
                    if(diretorios[i].nome == "") break;
                }
                next_id = i;

                /* ajusta bitmap e FAT*/
                livre--;
                FAT[i] = -1;
                bitmap[i] = 1;

                /* ajusta unordered map e grafo */
                d[argumentos[0]] = next_id;
                aux = argumentos[0].substr(0, argumentos[0].find_last_of("/"));
                j = d[aux];
                grafo[j].push_back(next_id);

                diretorios[next_id].nome = argumentos[0];
                diretorios[next_id].local = i;
                diretorios[next_id].criado = time(NULL);
                diretorios[next_id].modificado = time(NULL);
                diretorios[next_id].acessado = time(NULL);
                d[argumentos[0]] = next_id;
                num_diretorios++;
            }
            else
                cout << "Lotado" << endl;

        }
        else if(comando == "rmdir"){
            /*diretorio*/
            cin >> argumentos[0];
            i = d[argumentos[0]];
            rmdir(i);
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
            if(a.count(argumentos[0]) == 0)
            {
                //Cria arquivo num_arquivos
                aux = argumentos[0].substr(0, argumentos[0].find_last_of("/"));
                j = d[aux];

                arquivos[num_arquivos].nome = argumentos[0].substr(argumentos[0].find_last_of("/")+1, argumentos[0].length());
                arquivos[num_arquivos].conteudo = "";
                arquivos[num_arquivos].tamanho = 1;
                arquivos[num_arquivos].diretorio = aux;
                k = num_arquivos++;
                diretorios[j].arquivos.push_back(k);
                arquivos[k].criado = time(NULL);
            }
            else
                k = a[argumentos[0]];
            
            arquivos[k].acessado = time(NULL);
        }
        else if(comando == "rm"){
            /* arquivo */
            cin >> argumentos[0];
            rm(argumentos[0]);
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
            /*quantidade de diretórios,
            quantidade de arquivos, espaço livre, espaço desperdiçado (considerando o espaço a mais ne-
            cessário para cada arquivo ocupar exatamente múltiplos do tamanho de 1 bloco)*/
            cout << "quantidade de diretorios: " << num_diretorios <<  endl;
            cout << "quantidade de arquivo: " << num_arquivos <<  endl;
            cout << "quantidade de livre: " << livre <<  endl;
            cout << "quantidade de desperdiçado: " << 0 <<  endl;
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