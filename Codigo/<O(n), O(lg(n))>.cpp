#include <bits/stdc++.h>
#include <chrono>
using namespace std;
using namespace std::chrono;

#define foreach(exp) for(auto exp)
#define pv(v, s) for(int ye = 0; ye < s; ye++){ cout<<v[ye]<<" "; } cout<<endl<<endl;
#define pm(m) foreach(v : m){ foreach(e : v){ cout<<e<<" "; } cout<<endl; } cout<<endl;
#define pc(c) foreach(m : c){ foreach(v : m){ foreach(e : v){ cout<<e<<" "; } cout<<endl; } cout<<"--------------"<<endl; } cout<<endl;

#define test 0

inline int lg(int n){return (int) floor(log2(n));}

//arvore inicial
vector<int> *tree;

int n;

//vertices visitados pela dfs
int *dfsPath;//[2 * MAXSIZE - 1];
//nivel dos vertices visitados pela dfs
int *dfsLevel;//[MAXSIZE];
//indice da primeira ocorrencia de cada vertice dfs
int *dfsIndex;//[MAXSIZE];

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

//array para preprocessar o minimo para frente em cada bloco
int *minForward;
//array para preprocessar o minimo para tras em cada bloco
int *minBackward;

void build_blockmin(){
    int SIZE = 2 * n - 1;
    int blockSize = lg(SIZE);

    for(int i = 0; i < SIZE; i++){
        minBackward[i] = (i % blockSize == 0 ? dfsPath[i] : get_lowest(dfsPath[i], minBackward[i - 1]));
    }
    minForward[SIZE - 1] = dfsPath[SIZE - 1];
    for(int i = SIZE - 2; i >= 0; i--){
        minForward[i] = (i % blockSize == blockSize - 1 ? dfsPath[i] : get_lowest(dfsPath[i], minForward[i + 1]));
    }
}

void build_table(){
    int SIZE = 2 * n - 1;
    int blockSize = lg(SIZE);
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

int LCA(int u, int v){
    if(n == 1) return 0;
    int SIZE = 2 * n - 1;
    int blockSize = lg(SIZE);

    int l = dfsIndex[u], r = dfsIndex[v]; if(l > r) swap(l, r);
    int lblock = l / blockSize, rblock = r / blockSize;

    //caso 1: consulta num unico bloco
    if(lblock == rblock) {
        int ans = dfsPath[l];
        for(int i = l + 1; i <= r; i++) ans = get_lowest(ans, dfsPath[i]);
        return ans;
    }
    //caso 2: consulta em blocos adjacentes
    if(lblock + 1 == rblock) return get_lowest(minForward[l], minBackward[r]);
    //caso 3: consulta em blocos distantes
    return get_lowest(minForward[l], multiblock_query(lblock + 1, rblock - 1), minBackward[r]);
}

int main(){

    freopen("complete15.txt", "r", stdin);
    if(test) freopen("out3.txt", "w", stdout);
    position = 0;
    int s, u, v; cin>>n;

    //INITIALIZING MEMORY
    tree = new vector<int>[n];
    dfsPath  = new int[n << 1];
    dfsLevel = new int[n << 1];
    minForward  = new int[n << 1];
    minBackward = new int[n << 1];
    dfsIndex = new int[n];

    for(int i = 0; i < n; i++) tree[i].clear();

    for(int i = 0; i < n; i++) {
        cin>>s;
        while(s--){
            cin>>u;
            tree[i].push_back(u);
        }
    }

    //PREPROCESSING
    if(n > 1){
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        dfs();
        build_blockmin();
        build_table();
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>( t2 - t1 ).count();
        if(!test) cout << "PREPROCESSING: "<<duration<<endl;
    }
    else if(!test) cout << "PREPROCESSING: 0"<<endl;

    //QUERYING
    long long int numLCAS = 0;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    for(int i = 0; i < n; i++){
        for(int j = i; j < n; j++){
            if(!test) LCA(i, j);
            if(test) cout<<"LCA("<<i<<","<<j<<") = "<<LCA(i, j)<<endl;
            numLCAS++;
        }
    }
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>( t2 - t1 ).count();
    if(!test) cout<<"QUERY AVERAGE: "<<(duration / numLCAS)<<endl;
}
