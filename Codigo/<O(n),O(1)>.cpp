#include <bits/stdc++.h>
using namespace std;
#define foreach(exp) for(auto exp)

#define pv(v) foreach(e : v){ cout<<e<<" "; } cout<<endl;
#define pm(m) foreach(v : m){ foreach(e : v){ cout<<e<<" "; } cout<<endl; }
#define pc(c) foreach(m : c){ foreach(v : m){ foreach(e : v){ cout<<e<<" "; } cout<<endl; } cout<<"--------------"<<endl; } cout<<endl;
#define MAXSIZE 1010

inline int lg(int n){return (int) floor(log2(n));}

//arvore inicial
vector<int> tree[MAXSIZE];
int parent[MAXSIZE];

//vertices visitados pela dfs
int dfsPath[2 * MAXSIZE - 1];
//nivel dos vertices visitados pela dfs
int dfsLevel[MAXSIZE];
//indice da primeira ocorrencia de cada vertice dfs
int dfsIndex[MAXSIZE];

int n;

int position = 0;

void dfs(int u = 0, int level = 0){
    dfsPath[position] = u;
    dfsLevel[u] = level;
    dfsIndex[u] = position;
    (position++);

    foreach(v : tree[u]){
        dfs(v, level + 1);

        dfsPath[position] = u;
        (position++);
    }
}

int level(int u){return dfsLevel[dfsPath[u]];}

//dados dois vertices, retorna o de menor nivel
int get_lowest(int u, int v, int w = -1){
    if(w != -1) return get_lowest(dfsLevel[u] <= dfsLevel[w] ? u : w, v);
    return dfsLevel[u] <= dfsLevel[v] ? u : v;
}

//array para preprocessar o minimo para frente em cada bloco
int minForward[2 * MAXSIZE - 1];
//array para preprocessar o minimo para tras em cada bloco
int minBackward[2 * MAXSIZE - 1];

void build_blockmin(){
    int SIZE = 2 * n - 1;
    int blockSize = max(1, lg(SIZE) / 2);
    
    for(int i = 0; i < SIZE; i++){
        minBackward[i] = (i % blockSize == 0 ? dfsPath[i] : get_lowest(dfsPath[i], minBackward[i - 1]));
    }
    minForward[SIZE - 1] = dfsPath[SIZE - 1];
    for(int i = SIZE - 2; i >= 0; i--){
        minForward[i] = (i % blockSize == blockSize - 1 ? dfsPath[i] : get_lowest(dfsPath[i], minForward[i + 1]));
    }
}

//tabela de preprocessamento dos blocos
vector<vector<int> > table;

void build_table(){
    int SIZE = 2 * n - 1;
    int blockSize = max(1, lg(SIZE) / 2);
    int numBlocks = SIZE / blockSize + (SIZE % blockSize != 0);
    int cols = lg(numBlocks) + 1;

    //tabela de tamanho numBlocks x (lg(numBlocks) + 1)
    table = vector<vector<int> >(numBlocks);
    for(int i = 0; i < numBlocks; i++) table[i] = vector<int>(cols);

    for(int i = 0; i < numBlocks; i++) table[i][0] = minForward[i * blockSize];

    int power = 1;
    for(int i = 1; i < cols; i++){    //para cada coluna
        for(int j = 0, len = numBlocks - (power << 1); j <= len; j++){
            table[j][i] = get_lowest(table[j][i - 1], table[j + power][i - 1]);
        }
        power <<= 1;
    }
}

int multiblock_query(int lblock, int rblock){
    int num = rblock - lblock + 1;
    int col = floor(log2(num));
    return get_lowest(table[lblock][col], table[lblock + num - (1<<col)][col]);
}


vector<vector<vector<int> > > blocktable;
vector<int> blockIndex;

void build_blocktable(){
    int SIZE = 2 * n - 1;
    int blockSize = max(1, lg(SIZE) / 2);

    vector<int> binary(SIZE);
    int sum = 0;

    for(int i = 0, len = SIZE + (blockSize - (SIZE % blockSize)); i < len; i++){
        if(i >= SIZE){
            sum <<= SIZE - i;
            blockIndex.push_back(sum);
            break;
        }

        if(i % blockSize == 0) binary[i] = 0;
        else binary[i] = level(i) > level(i - 1);

        sum = sum << 1 | binary[i];

        if(i % blockSize == blockSize - 1){
            blockIndex.push_back(sum);
            sum = 0;
        }
    }

    int numBlocks = 1 << (blockSize - 0);
    int cols = lg(blockSize) + 1;
    blocktable = vector<vector<vector<int> > > (numBlocks);

    int temp[blockSize], real[blockSize];

    for(int blockNum = 0; blockNum < numBlocks; blockNum++){

        vector<vector<int> > &block = blocktable[blockNum];

        block = vector<vector<int> > (blockSize);
        for(int i = 0; i < blockSize; i++) block[i] = vector<int> (cols);

        int aux = blockNum;
        real[0] = temp[0] = 0;
        for(int i = blockSize - 1; i >= 0; i--) {
            temp[i] = aux & 1;
            aux >>= 1;
        }

        for(int i = 1; i < blockSize; i++) {
            real[i] = real[i - 1] + (temp[i] ? 1 : -1);
        }

        for(int i = 0; i < blockSize; i++) {
            block[i][0] = i;
        }

        int power = 1;
        for(int i = 1; i < cols; i++){    //para cada coluna
            for(int j = 0, len = blockSize - (power << 1); j <= len; j++){
                block[j][i] = real[ block[j][i - 1] ] < real[ block[j + power][i - 1] ] ?
                    block[j][i - 1] : block[j + power][i - 1];
            }
            power <<= 1;
        }
    }
}

int block_query(int block, int l, int r){

    vector<vector<int> > &B = blocktable[blockIndex[block]];
    int blockOffset = block * ( max(1, lg(2 * n - 1) / 2) );
    int num = r - l + 1;
    int col = floor(log2(num));
    
    return get_lowest(
        dfsPath[B[l][col] + blockOffset],
        dfsPath[B[l + num - (1<<col)][col] + blockOffset]
    );
}


int LCA(int u, int v){

    int SIZE = 2 * n - 1;
    int blockSize = max(1, lg(SIZE) / 2);

    int l = dfsIndex[u], r = dfsIndex[v]; if(l > r) swap(l, r);

    int lblock = l / blockSize, rblock = r / blockSize;
    
    //se n < 9, o bloco tem tamanho 1 e nao ha necessidade de preprocessar blocos individuais
    if(n < 9){ //lg(2 * 8 - 1) / 2 = lg(15) / 2 = 3 / 2 = 1
        //caso 1: consulta num unico bloco
        if(lblock == rblock) return table[lblock][0];
        //caso 2: consulta em blocos diferentes
        return multiblock_query(lblock + 1, rblock - 1);
    }

    //caso 1: consulta num unico bloco
    if(lblock == rblock) {
        return block_query(lblock, l % blockSize, r % blockSize);
    }

    int lmin = minForward[l];
    int rmin = minBackward[r];
    //caso 2: consulta em blocos adjacentes
    if(lblock + 1 == rblock) {
        return get_lowest(lmin, rmin);
    }
    //caso 3: consulta em blocos distantes
    return get_lowest(lmin, multiblock_query(lblock + 1, rblock - 1), rmin);
}


void init(){
    position = 0;
    int s, q, u, v; cin>>n;
    for(int i = 0; i < n; i++) tree[i].clear();

    for(int i = 0; i < n; i++) parent[i] = i;
    for(int i = 0; i < n; i++) {
        cin>>s;
        while(s--){
            cin>>q; q--;
            parent[q] = i;
            tree[i].push_back(q);
        }
    }

    int root = 0;
    while(root != parent[root]) root = parent[root];


    dfs(root);
    build_blockmin();
    build_table();

    if(n > 8) build_blocktable();

    cin>>q;
    while(q--){
        cin>>u>>v;
        cout<<(LCA(u - 1, v - 1) + 1)<<endl;
    }
}

int main(){
    int t;
    cin>>t;
    for(int caso = 1; caso <= t; caso++){
        cout<<"Case "<<caso<<":"<<endl;
        init();
    }
}
