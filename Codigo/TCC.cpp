#include <bits/stdc++.h>
using namespace std;
#define foreach(exp) for(auto exp)
#define MAXSIZE 1010
#define pv(v, s) for(int ye = 0; ye < s; ye++){ cout<<v[ye]<<" "; } cout<<endl<<endl;
#define pm(m) foreach(v : m){ foreach(e : v){ cout<<e<<" "; } cout<<endl; } cout<<endl;
#define pc(c) foreach(m : c){ foreach(v : m){ foreach(e : v){ cout<<e<<" "; } cout<<endl; } cout<<"--------------"<<endl; } cout<<endl;


inline int lg(int n){return (int) floor(log2(n));}

//arvore inicial
vector<int> tree[MAXSIZE];
int parent[MAXSIZE];

int n;

//vertices visitados pela dfs
int dfsPath[2 * MAXSIZE - 1];
//nivel dos vertices visitados pela dfs
int dfsLevel[MAXSIZE];
//indice da primeira ocorrencia de cada vertice dfs
int dfsIndex[MAXSIZE];

int position;

void dfs(int u = 0, int level = 0){
    dfsPath[position] = u;
    dfsLevel[u] = level;
    dfsIndex[u] = position++;

    foreach(v : tree[u]){
        dfs(v, level + 1);
        dfsPath[position++] = u;
    }
}

//tabela para preprocessar cada bloco internamente
vector<vector<vector<int> > > blocktable;
//tabela para preprocessar cada bloco
vector<vector<int> > table;

//dados dois vertices, retorna o de menor nivel
int get_lowest(int u, int v, int w = -1){
    if(w != -1) return get_lowest(dfsLevel[u] <= dfsLevel[w] ? u : w, v);
    return dfsLevel[u] <= dfsLevel[v] ? u : v;
}

void build_blocktable(){
    int SIZE = 2 * n - 1;
    int blockSize = lg(SIZE);
    int numBlocks = SIZE / blockSize + (SIZE % blockSize != 0);
    int blockWidth = lg(blockSize) + 1;
    //numBlocks blocos de tamanho blockSize x blockWidth
    blocktable = vector<vector<vector<int> > >(numBlocks);
    for(int i = 0; i < numBlocks; i++) blocktable[i] = vector<vector<int> >(blockSize);
    for(int i = 0; i < numBlocks; i++) for(int j = 0; j < blockSize; j++) blocktable[i][j] = vector<int>(blockWidth);

    position = 0;
    for(int block = 0; block < numBlocks; block++){
        for(int i = 0; i < blockSize; i++) blocktable[block][i][0] = dfsPath[min(position++, 2 * n - 2)];
        int power = 1;
        for(int i = 1; i < blockWidth; i++){    //para cada coluna
            for(int j = 0, len = blockSize - (power << 1); j <= len; j++){
                blocktable[block][j][i] = get_lowest(blocktable[block][j][i - 1], blocktable[block][j + power][i - 1]);
            }
            power <<= 1;
        }
    }
}

int block_query(int block, int l, int r){
    int num = r - l + 1;
    int col = floor(log2(num));
    return get_lowest(blocktable[block][l][col], blocktable[block][l + num - (1<<col)][col]);
}

void build_table(){
    int SIZE = 2 * n - 1;
    int blockSize = lg(SIZE);
    int numBlocks = SIZE / blockSize + (SIZE % blockSize != 0);
    int cols = lg(numBlocks) + 1;

    //tabela de tamanho numBlocks x (lg(numBlocks) + 1)
    table = vector<vector<int> >(numBlocks);
    for(int i = 0; i < numBlocks; i++) table[i] = vector<int>(cols);

    for(int i = 0; i < numBlocks; i++) table[i][0] = block_query(i, 0, blockSize - 1);

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

int LCA(int u, int v){
    int SIZE = 2 * n - 1;
    int blockSize = lg(SIZE);

    int l = dfsIndex[u], r = dfsIndex[v]; if(l > r) swap(l, r);
    int lblock = l / blockSize, rblock = r / blockSize;

    //caso 1: consulta num unico bloco
    if(lblock == rblock) return block_query(lblock, l % blockSize, r % blockSize);

    int lmin = block_query(lblock, l % blockSize, blockSize - 1);
    int rmin = block_query(rblock, 0, r % blockSize);
    //caso 2: consulta em blocos adjacentes
    if(lblock + 1 == rblock) return get_lowest(lmin, rmin);
    //caso 3: consulta em blocos distantes
    return get_lowest(lmin, multiblock_query(lblock + 1, rblock - 1), rmin);
}

void init(){
    position = 0;
    int s, q, u, v; cin>>n;
    for(int i = 0; i < n; i++) tree[i].clear();
    /*
    tree[0].push_back(1);tree[0].push_back(2);tree[0].push_back(3);
    tree[2].push_back(4);
    tree[3].push_back(5);tree[3].push_back(6);
    tree[6].push_back(7);tree[6].push_back(8);
    tree[8].push_back(9);
    */

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

    if(n > 1){
        dfs(root);
        build_blocktable();
        build_table();
    }
    //pv(dfsPath, 2 * n - 1);
    //pv(index, n);
    //pm(table);
    //pc(blocktable);

    cin>>q;
    while(q--){
        cin>>u>>v;

        if(n > 1) cout<<(LCA(u - 1, v - 1) + 1)<<endl;
        else cout<<1<<endl;
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
