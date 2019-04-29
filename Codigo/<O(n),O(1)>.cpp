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
    //cout<<"BLOCKSIZE 1: "<<blockSize<<endl;
    for(int i = 0; i < SIZE; i++){
        minBackward[i] = (i % blockSize == 0 ? dfsPath[i] : get_lowest(dfsPath[i], minBackward[i - 1]));
    }
//cout<<"oi\n";
    minForward[SIZE - 1] = dfsPath[SIZE - 1];
    for(int i = SIZE - 2; i >= 0; i--){
        minForward[i] = (i % blockSize == blockSize - 1 ? dfsPath[i] : get_lowest(dfsPath[i], minForward[i + 1]));
    }

/*
    cout<<"blockSize = "<<blockSize<<endl;

    for(int i = 0; i < SIZE; i++){
        if(i % blockSize == 0) cout<<"| ";
        cout<<dfsPath[i]<<" ";
    } cout<<endl;

    for(int i = 0; i < SIZE; i++){
        if(i % blockSize == 0) cout<<"| ";
        cout<<minBackward[i]<<" ";
    } cout<<endl;

    for(int i = 0; i < SIZE; i++){
        if(i % blockSize == 0) cout<<"| ";
        cout<<minForward[i]<<" ";
    } cout<<endl;
    */
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

    //pm(table); cout<<"\n\n";
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

    //cout<<"BLOCKSIZE 2: "<<blockSize<<endl;

    vector<int> binary(SIZE);

    int sum = 0;

    //cout<<"Level: "; for(int i = 0; i < SIZE; i++) cout<<level(i)<<" "; cout<<endl;

    for(int i = 0, len = SIZE + (blockSize - (SIZE % blockSize)); i < len; i++){

        if(i >= SIZE){
            sum <<= SIZE - i;
            //for(int it = SIZE - i; it >= 0; it--) cout<<"0 "; cout<<endl;
            blockIndex.push_back(sum);
            break;
        }

        if(i % blockSize == 0) binary[i] = 0;
        else binary[i] = level(i) > level(i - 1);

        //cout<<binary[i]<<" ";

        sum = sum << 1 | binary[i];

        if(i % blockSize == blockSize - 1){
            //cout<<"| ";
            blockIndex.push_back(sum);
            sum = 0;
        }
    }

    //cout<<endl;

    //pv(blockIndex);

    int numBlocks = 1 << (blockSize - 0);
    int cols = lg(blockSize) + 1;
    blocktable = vector<vector<vector<int> > > (numBlocks);

    int temp[blockSize], real[blockSize];
    //cout<<"numBlocks = "<<numBlocks<<endl;

    for(int blockNum = 0; blockNum < numBlocks; blockNum++){

        vector<vector<int> > &block = blocktable[blockNum];

        block = vector<vector<int> > (blockSize);
        for(int i = 0; i < blockSize; i++) block[i] = vector<int> (cols);

        int aux = blockNum;
        //cout<<"building table "<<aux<<endl;
        real[0] = temp[0] = 0;
        for(int i = blockSize - 1; i >= 0; i--) {
            temp[i] = aux & 1;
            aux >>= 1;
        }

        //cout<<"temp:"; pv(temp);

        for(int i = 1; i < blockSize; i++) {
            real[i] = real[i - 1] + (temp[i] ? 1 : -1);
        }

        for(int i = 0; i < blockSize; i++) {
            block[i][0] = i;
        }
        //cout<<endl;
        //cout<<"Block: "; for(int i = 0; i < blockSize; i++) cout<<block[i][0]<<" "; cout<<endl;
        //cout<<"Binary:"; pv(temp);
        //cout<<"Real:  "; pv(real);


        int power = 1;
        for(int i = 1; i < cols; i++){    //para cada coluna
            for(int j = 0, len = blockSize - (power << 1); j <= len; j++){
                block[j][i] = real[ block[j][i - 1] ] < real[ block[j + power][i - 1] ] ?
                    block[j][i - 1] : block[j + power][i - 1];
            }
            power <<= 1;
        }
        //pm(block);

    }
    //pc(blocktable);
}

int block_query(int block, int l, int r){
    //cout<<"L = "<<l<<", R = "<<r<<endl;
    //cout<<"block "<<block<<" corresponds to blocktable["<<blockIndex[block]<<"]\n";

    vector<vector<int> > &B = blocktable[blockIndex[block]];

    //pm(blocktable[block]);
    int blockOffset = block * ( max(1, lg(2 * n - 1) / 2) );
    int num = r - l + 1;
    int col = floor(log2(num));
    //cout<<"coluna "<<col<<endl;
    //cout<<dfsPath[B[l][col] + blockOffset]<<", "<<dfsPath[B[l + num - (1<<col)][col] + blockOffset]<<endl;

    //int minimum =
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
    //cout<<"Lblock = "<<lblock<<", Rblock = "<<rblock<<endl;

    //caso 1: consulta num unico bloco
    if(lblock == rblock) {
        //cout<<"caso 1\n";
        return block_query(lblock, l % blockSize, r % blockSize);
    }

    int lmin = minForward[l];
    int rmin = minBackward[r];
    //caso 2: consulta em blocos adjacentes
    if(lblock + 1 == rblock) {
        //cout<<"caso 2\n";
        return get_lowest(lmin, rmin);
    }
    //caso 3: consulta em blocos distantes
    //cout<<"caso 3\n";
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
        build_blockmin();
        build_table();
        build_blocktable();
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
        //dfs();
        //build_blocktable();
        //build_table();
        //for(int i = 0; i < 10; i++){
        //    for(int j = i; j < 10; j++){
        //        cout<<"LCA("<<i<<","<<j<<") = "<<LCA(i,j)<<endl<<endl;
        //    }
        //}
    }

}



