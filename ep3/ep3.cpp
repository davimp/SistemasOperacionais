#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <time.h>
#define MAXN 25000 /* quantidade de blocos */
#define BLOCO 4000 /* cada bloco pode ter até quatro mil caracteres */
#define MIN(x, y) (x < y) ? x : y
#define MAX(x, y) (x > y) ? x : y
#define FATBIT 45

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
string BLOCOS[MAXN];
int livre, desperdicio;
int num_diretorios, num_arquivos;
Diretorio diretorios[MAXN];
Arquivo arquivos[MAXN];
vector<int> grafo[MAXN];
unordered_map<string, int> d, a; /* liga o caminho do diretório/arquivo ao seu id */

int proximo_livre(){
    int x;
    for(x = FATBIT; x < MAXN; x++){
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

void find(int id, string nome){

    for(int it : diretorios[id].arquivos){
        if(arquivos[it].nome == nome) cout << diretorios[id].nome << "/" << arquivos[it].nome << endl;
    }

    for(int it : grafo[id]){
        find(it, nome);
    }
}

void rm(string n){
    string aux;
    int i,j,k;

    /* pegas os ids */
    i = a[n];
    aux = n.substr(0, n.find_last_of("/"));
    a[n] = 0;
    j = d[aux];
    diretorios[j].acessado = time(NULL);
    diretorios[j].modificado = time(NULL);

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

void rmdir(string n){
    string aux;
    int i,j,k, id;

    id = d[n];
    d[n] = 0;
    aux = n.substr(0, n.find_last_of("/"));
    j = d[aux];
    diretorios[j].acessado = time(NULL);
    diretorios[j].modificado = time(NULL);

    /* apaga os diretorios filhos */
    for(int it : grafo[id]){
        rmdir(diretorios[it].nome);
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

void limpa(){
    int i, j;
    num_diretorios = num_arquivos = 0;
    livre = 25000; /* quantos blocos estão livres */
    desperdicio = 0;
    /* inicia os vetores de diretorios e arquivos */
    for(i = 0; i < MAXN; i++){
        grafo[i].clear();
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
    diretorios[0].local = FATBIT;
    diretorios[0].acessado = time(NULL);
    diretorios[0].criado = time(NULL);
    diretorios[0].modificado = time(NULL);
    num_diretorios++;

    /* espaço ocupado por fat + bitmap + primeiro diretorio no início do arquivo */

    for(i = 0; i <= FATBIT; i++){
        bitmap[i] = 1;
        FAT[i] = -1;
        BLOCOS[i] = "";
    }
    for(i = FATBIT+1; i < MAXN; i++){
        bitmap[i] = 0;
        FAT[i] = -1;
        BLOCOS[i] = "";
    }
    livre -= FATBIT;
}

void umount(string nome_arq)
{ 
    int i;
    char saux[1000];
    string s;
    fstream arq;

    remove(nome_arq.data());
    arq.open(nome_arq, fstream::out | fstream::in | fstream::app);
    /* writer */
    int ultimo_bloco = FATBIT;
    
    // Bitmap
    for(i = 0; i < 25000; i++)
        arq << bitmap[i];
    s = "";
    for(i = 0; i < 2999; i++)
        s += "_";
    arq << s + "\n";
    
    // FAT
    for(i = 0; i < 25000; i++)
    {
        sprintf(saux, "%05d\n", FAT[i]);
        arq << string(saux);
    }
    s = "";
    for(i = 0; i < 1999; i++)
        s += "_";
    arq << s + "\n";

    // Diretorios
    for(i = 0; i < num_diretorios; i++)
    {
        int local = diretorios[i].local;
        ultimo_bloco = MAX(ultimo_bloco, local);
        s = "1\n";
        string nome;

        if(i == 0)
        {            
            sprintf(saux, "%05d ", diretorios[0].local);
            s += string(saux);

            nome = diretorios[0].nome;
            nome = "/";
            for(int ii = 0; ii < 20 - nome.length(); ii++)
                s += " ";
            s += nome + " ";

            sprintf(saux, "%09d %010d %010d %010d\n", -1,  diretorios[0].criado , diretorios[0].modificado , diretorios[0].acessado);
            s += string(saux);            
        }

        for(int z : grafo[i])
        {
            ultimo_bloco = MAX(ultimo_bloco, diretorios[z].local);
            sprintf(saux, "%05d ", diretorios[z].local);
            s += string(saux);

            nome = diretorios[z].nome;
            nome = nome.substr(nome.find_last_of("/")+1, nome.length());
            for(int ii = 0; ii < 20 - nome.length(); ii++)
                s += " ";
            s += nome + " ";

            sprintf(saux, "%09d %010d %010d %010d\n", -1,  diretorios[z].criado , diretorios[z].modificado , diretorios[z].acessado);
            s += string(saux);
        }

        for(int z : diretorios[i].arquivos)
        {
            sprintf(saux, "%05d ", arquivos[z].inicio);
            s += string(saux);

            nome = arquivos[z].nome;
            /*nome = nome.substr(nome.find_last_of("/")+1, nome.length());*/
            for(int ii = 0; ii < 20 - nome.length(); ii++)
                s += " ";
            s += nome + " ";

            sprintf(saux, "%09d %010d %010d %010d\n", arquivos[z].tamanho,  arquivos[z].criado , arquivos[z].modificado , arquivos[z].acessado);
            s += string(saux);

        }

        BLOCOS[local] = s;
    }

    for(i = 0; i < num_arquivos; i++)
    {
        int local = arquivos[i].inicio;
        
        ultimo_bloco = MAX(ultimo_bloco, local);
        s = "0\n";

        s += arquivos[i].conteudo;

        //arq << s;

        while(local != -1)
        {
            BLOCOS[local] = s.substr(0, MIN(s.length(), BLOCO-1));
            BLOCOS[local] += "\n";
            s = s.substr(MIN(s.length(), BLOCO-1), s.length());
            local = FAT[local];
            ultimo_bloco = MAX(ultimo_bloco, local);
        }
    }

    desperdicio = 2000 + 2999;
    for(i = FATBIT; i <= ultimo_bloco; i++)
    {
        arq << BLOCOS[i];
        
        s = "";

        // completa o bloco
        for(int c = 0; c < 4000-((int) BLOCOS[i].length())-1; c++)
            s += "_"; 
        if(BLOCOS[i].length() < 4000)
            s += "\n";
        
        if(bitmap[i])
            desperdicio += 4000 - (BLOCOS[i].length());

        arq << s;
    }
    arq.close();

    cerr << "desper: " << desperdicio << endl;


}

int main(int argc, char *argv[]){
    int i,j,k, next_id, blocos, cont;
    string nome_arq, s, content, aux;
    string linha_lida, comando, argumentos[10];
    char saux[1000];
    fstream arq, original;
    streampos pos;

    limpa();

    /* shell */
    while(1)
    {
        cout << "[ep3]: ";
        cin >> comando;

        if(comando == "mount"){
            /* parser */
            /* arquivo */
            cin >> argumentos[0];
            nome_arq = argumentos[0];
            /*cout << "monte " << argumentos[0] << endl;*/
            limpa();
            //continue;
            arq.open(argumentos[0], fstream::out | fstream::in | fstream::app);

            //arq >> s;

            // se o arquivo estiver vazio
            if(!(arq >> s))
            {
                cerr << "ARQUIVO VAZIO" << endl;
                arq.close();
                continue;
            }

            arq.seekg(0, fstream::beg);
            arq >> s;

            for(i = 0; i < 25000; i++)
                bitmap[i] = (s[i] == '1');
            
            for(i = 0; i < 25000; i++)
            {
                arq >> s;
                FAT[i] = atoi(s.data());
            }

            arq >> s;
            
            int bloco_estou = FATBIT;
            int id = 0;
            string nome;

            num_diretorios = 0;
            desperdicio = 2000 + 2999;

            queue<int> fila_dir, fila_arq;

            fila_dir.push(id);

            while(!fila_dir.empty())
            {
                id = fila_dir.front();
                fila_dir.pop();

                arq.seekg(diretorios[id].local*BLOCO, fstream::beg);

                arq >> s;

                if(s == "1")
                {
                    arq >> s;
                    desperdicio += 4000;
                    desperdicio -= 2;
                    while(s[0] != '_')
                    {
                        int local = atoi(s.data());
                        arq >> s;
                        nome = s;
                        
                        arq >> s;
                        if(atoi(s.data()) == -1)
                        {
                            if(nome != "/")
                                diretorios[num_diretorios].nome = diretorios[id].nome + ((id == 0)? "" : "/") + nome;
                            else
                                diretorios[num_diretorios].nome = "/";
                            d[diretorios[num_diretorios].nome] = num_diretorios;
                            cerr << "dir: " << diretorios[num_diretorios].nome  << " " << num_diretorios << endl;

                            arq >> s;
                            diretorios[num_diretorios].criado = atoi(s.data());
                            arq >> s;
                            diretorios[num_diretorios].modificado = atoi(s.data());
                            arq >> s;
                            diretorios[num_diretorios].acessado = atoi(s.data());

                            if(diretorios[num_diretorios].nome != "/")
                            {
                                grafo[id].push_back(num_diretorios);
                                diretorios[num_diretorios].local = local;
                                fila_dir.push(num_diretorios);
                            }
                            num_diretorios++;
                        }
                        else
                        {
                            cerr << "nome: " << nome << endl;
                            diretorios[id].arquivos.push_back(num_arquivos);
                            arquivos[num_arquivos].inicio = local;
                            arquivos[num_arquivos].nome = nome;

                            a[diretorios[id].nome + ((id == 0)? "" : "/") + arquivos[num_arquivos].nome] = num_arquivos;

                            arquivos[num_arquivos].tamanho = atoi(s.data());
                            arq >> s;
                            arquivos[num_arquivos].criado = atoi(s.data());
                            arq >> s;
                            arquivos[num_arquivos].modificado = atoi(s.data());
                            arq >> s;
                            arquivos[num_arquivos].acessado = atoi(s.data());
                            
                            fila_arq.push(num_arquivos);
                            num_arquivos++;
                        }
                        desperdicio -= 70;
                        arq >> s;
                    }
                }
                else
                {
                    cerr << "ESSA MENSAGEM SÓ APARECERÁ EM CASO DE SINTAXE ERRADA NO ARQUIVO" << endl;
                }
            }


            while(!fila_arq.empty())
            {
                id = fila_arq.front();
                fila_arq.pop(); 

                arquivos[id].conteudo = "";

                int local = arquivos[id].inicio;

                arq.seekg(local*BLOCO, fstream::beg);
                arq >> s;

                if(arquivos[id].tamanho)
                {
                    arq >> s;
                    arquivos[id].conteudo += s;
                    cerr << "id: " << id << endl;
                }

                local = FAT[local];

                while(local != -1)
                {
                    arq >> s;
                    arquivos[id].conteudo += s;
                    local = FAT[local];
                }
                if(arquivos[id].tamanho)
                {
                    desperdicio += 4000 - (s.length()+1);
                    if(FAT[arquivos[id].inicio] == -1)
                        desperdicio -= 2;
                    cerr << "comprimento: " << s.length() << endl;
                }
                else
                    desperdicio += 4000-1-2;
                
            }

            /* atualiza quantos blocos tem livre */
            livre = 25000;
            for(i = 0; i < MAXN; i++){
                if(bitmap[i]){
                    livre--;
                }
            }
           arq.close();
            cerr << "desper: " << desperdicio << endl;
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
                diretorios[j].acessado = time(NULL);
                diretorios[j].modificado = time(NULL);

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
            
            umount(nome_arq);
        }
        else if(comando == "mkdir"){
            /* diretorio */
            cin >> argumentos[0];

            if(d.count(argumentos[0])) /* caso já tenha sido criado, continua */
                continue;

            i = proximo_livre(); /* diretórios ocuparão um bloco apenas */

            /* crio o diretorio no bloco i */
            if(i < MAXN)
            {
                /* acha qual o id do diretorio */
                for(k = 0; k < MAXN; k++){
                    if(diretorios[k].nome == "") break;
                }
                next_id = k;

                /* ajusta bitmap e FAT*/
                livre--;
                FAT[i] = -1;
                bitmap[i] = 1;

                /* ajusta unordered map e grafo */
                d[argumentos[0]] = next_id;
                aux = argumentos[0].substr(0, argumentos[0].find_last_of("/"));
                j = d[aux];
                grafo[j].push_back(next_id);
                diretorios[j].acessado = time(NULL);
                diretorios[j].modificado = time(NULL);

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
            umount(nome_arq);

        }
        else if(comando == "rmdir"){
            /*diretorio*/
            cin >> argumentos[0];
            rmdir(argumentos[0]);
            umount(nome_arq);
        }
        else if(comando == "cat"){
            /* arquivo */
            cin >> argumentos[0];
            j = a[argumentos[0]];
            cerr << "debug " << j << " " << argumentos[0] << endl;
            cout << arquivos[j].conteudo << endl;
            umount(nome_arq);
        }
        else if(comando == "touch"){
            /* arquivo */
            cin >> argumentos[0];
            
            k = proximo_livre();

            if(a.count(argumentos[0]) == 0)
            {
                /* acha o qual será o id do arquivo */
                for(i = 0; i < MAXN; i++){
                    if(arquivos[i].nome == "") break;
                }
                next_id = i;
  
                /* ajusta bitmap e FAT*/
                bitmap[k] = 1;
                FAT[k] = -1;
  
                /* cria arquivo em next_id */
                aux = argumentos[0].substr(0, argumentos[0].find_last_of("/"));
                j = d[aux];
                diretorios[j].acessado = time(NULL);
                diretorios[j].modificado = time(NULL);

                arquivos[next_id].nome = argumentos[0].substr(argumentos[0].find_last_of("/")+1, argumentos[0].length());
                arquivos[next_id].conteudo = "";
                arquivos[next_id].tamanho = 0;
                arquivos[next_id].inicio = k;
                arquivos[next_id].diretorio = aux;
                a[argumentos[0]] = next_id;
                diretorios[j].arquivos.push_back(next_id);
                num_arquivos++;
                arquivos[next_id].criado = time(NULL);
                arquivos[next_id].modificado = time(NULL);
                arquivos[next_id].acessado = time(NULL);
                k = next_id;
            }
            else
                k = a[argumentos[0]];
            
            arquivos[k].acessado = time(NULL);
            umount(nome_arq);
        }
        else if(comando == "rm"){
            /* arquivo */
            cin >> argumentos[0];
            cout << "apagando arquivo " << argumentos[0] << endl;
            rm(argumentos[0]);
            umount(nome_arq);
        }
        else if(comando == "ls"){
            /* diretorio */
            cin >> argumentos[0];
            j = d[argumentos[0]];
            imprime_arquivos(j);
        }
        else if(comando == "find"){
            /* diretorio arquivo */
            cin >> argumentos[0] >> argumentos[1];
            j = d[argumentos[0]];
            find(j, argumentos[1]);
        }
        else if(comando == "df"){
            /*quantidade de diretórios,
            quantidade de arquivos, espaço livre, espaço desperdiçado (considerando o espaço a mais ne-
            cessário para cada arquivo ocupar exatamente múltiplos do tamanho de 1 bloco)*/
            cout << "quantidade de diretorios: " << num_diretorios <<  endl;
            cout << "quantidade de arquivo: " << num_arquivos <<  endl;
            cout << "quantidade de livre: " << livre <<  endl;
            cout << "quantidade de desperdiçado: " << desperdicio <<  endl;
        }
        else if(comando == "umount"){

            umount(nome_arq);
          
        }
        else if(comando == "sai"){
            cout << "SAÍRA" << endl;
            return 0;
            break;
        }
        else{
            cout << "comando inválido!\n";
        }
    }

    return 0;
}
