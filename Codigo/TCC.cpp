#include <bits/stdc++.h>
using namespace std;
#define debug 1
#define foreach(exp) for(auto exp)
#define pa(a) if(debug){cout<<#a ":\t"; foreach(x : a) cout<<x<<" "; cout<<endl;}
#define pb(a) if(debug){ cout<<#a ": ";             \
    for(int i = 0; i < 2 * N - 1; i++) {            \
        if(i % lg(2 * N - 1) == 0 && i) cout<<"| "; \
        cout<<a[i]<<" ";                            \
    } cout<<endl;                                   \
}

int lg(int n){
    return (int) floor(log2(n));
}


int N, bucketSize, bucketCount;
vector<vector<int> > tree;
vector<vector<int> > table;
vector<int> level, node, index, minForward, minBackward;

///index: guarda posicao da primeira ocorrencia
///de um no na dfs

int lowest(int i, int j, int k = -1){
    int db = level[i] <= level[j] ? node[i] : node[j];
    return k == -1? db : level[db] <= level[k] ? node[db] : node[k];
}

///inicializacao da arvore
void init(){
    tree.clear(); table.clear(); level.clear(); node.clear();
    index.clear(); minForward.clear(); minBackward.clear();
    cin>>N;
    tree = vector<vector<int> >(N);
    index = vector<int> (N, INT_MAX);
    int c, f;
    for(int i = 0; i < N; i++){
        cin>>c;
        //cout<<i<<" tem "<<c<<" filhos\n";
        while(c--){
            cin>>f;
            tree[i].push_back(f);
        }
    }
    pa(tree[0]);
}

inline int h(int u){return level[index[u]];}

///apos essa dfs, teremos duas listas:
///node: lista composta de todo caminho da dfs
///level: lista do nivel de cada no em node
///ambos tem tamanho 2n-1
void dfs(int u, int l = 0){
    node.push_back(u); level.push_back(l);
    index[u] = min(index[u], (int) node.size() - 1);
    foreach(v : tree[u]) {
        dfs(v, l + 1);
        node.push_back(u); level.push_back(l);
    }
}



void build_minimum(){
    bucketSize = lg(2 * N - 1);
    bucketCount = ((2 * N - 1) / bucketSize) + ((2 * N - 1) % bucketSize != 0);
    table = vector< vector<int> >(bucketCount);  table[0] = vector<int>();

    minForward = vector<int>(2 * N - 1, INT_MAX);
    minBackward = vector<int>(2 * N - 1, INT_MAX);

    ///gerar os minimos para tras
    for(int bucket = 0; bucket < bucketCount; bucket++){
        //cout<<"Bucket "<<bucket<<endl;
        int i = bucket * bucketSize;
        minBackward[i] = node[i++];
        for(int len = min(i + bucketSize - 1, 2 * N - 1); i < len; i++){
            //cout<<"mB[i] = "<<level[i]<<" <= "<<level[ minBackward[i - 1] ]<<" ? "<<node[i]<<" : "<<minBackward[i - 1]<<endl;

            minBackward[i] = h(node[i]) < h(minBackward[i - 1]) ? node[i] : minBackward[i - 1];
        }
        ///preenchendo primeira linha da sparse table
        table[0].push_back(minBackward[i - 1]);
    }

    ///gerar os minimos para frente
    for(int bucket = 0; bucket < bucketCount; bucket++){
        //cout<<"Bucket "<<bucket<<endl;
        int i = min((bucket + 1) * bucketSize, 2 * N - 1) - 1;
        //cout<<"i = "<<i<<endl;
        minForward[i] = node[i--];
        for(int len = bucket * bucketSize; i >= len; i--){
            //cout<<"mF["<<i<<"] = "<<level[index[minForward[i + 1]]] <<" < " << level[i]<<" ?"<<endl;
            //cout<<"\t"<<minForward[i + 1]<<" : "<<node[i]<<endl;

            //cout<<"mF["<<i<<"] = "<<level[i]<<" <= "<<level[ minForward[i + 1] ]<<" ? "<<node[i]<<" : "<<minBackward[i - 1]<<endl;
            minForward[i] = h(node[i]) < h(minForward[i + 1]) ? node[i] : minForward[i + 1];

        }
    }
}

///constroi uma sparse table de tamanho (n/lg(n)) x lg(n/lg(n))
void build_sparse_table(){

    for(int i = 1, len = lg(table[0].size()) + 1; i < len; i++){
        table[i] = vector<int>();               ///2^i
        for(int j = 0, len = table[0].size() - (1 << i) + 1; j < len; j++){
            //cout<<"limit = "<<len<<endl;
            //cout<<"table["<<i<<"]["<<j<<"] = min("<<table[i - 1][j]<<", "<<table[i - 1][j + pow(2, i - 1)]<<")\n";
            table[i].push_back(min(table[i - 1][j], table[i - 1][j + (1 << (i - 1))]));
        }
    }
    //cout<<endl;
    /*
    for(int i = 0; i < table.size(); i++){
        for(int j = 0; j < table[i].size(); j++){
            cout<<table[i][j]<<" ";
        }
        cout<<endl;
    }
    */
}

///o LCA tem 2 casos:
///1 - ambos os nos no mesmo bucket
///2 - nos em buckets diferentes
int LCA(int u, int v){
    //cout<<"LCA("<<u<<","<<v<<")"<<endl;
    ///extraimos o index real de u e v na dfs
    int uindex = index[u], vindex = index[v];
    if(uindex > vindex) swap(uindex, vindex);

    //cout<<"\tindex["<<u<<"] = "<<uindex<<", index["<<v<<"] = "<<vindex<<endl;
    ///calculamos em qual bucket eles estao
    int ubucket = uindex / bucketSize, vbucket = vindex / bucketSize;
    //cout<<"\tbucket["<<u<<"] = "<<ubucket<<", bucket["<<v<<"] = "<<vbucket<<endl;
    ///caso 1: O(lg(n))
    if(ubucket == vbucket){
        //cout<<"caso1"<<endl;
        int lca = uindex;
        for(int i = uindex + 1; i <= vindex; i++){
            lca = level[ node[i] ] < level[ node[lca] ] ? i : lca;
        }
        return node[lca];
    }
    ///caso 2: O(1)
    else{

        /*cout<<"\tCaso 2: ";
        for(int i = 0; i < 2 * N - 1; i++){
            if(i == uindex) cout<<"| ";
            cout<<node[i]<<" ";
            if(i == vindex) cout<<"| ";
        }cout<<endl;
        */

        /// [  u---][][][][][---v  ]
        int leftMin = minForward[uindex], rightMin = minBackward[vindex];
        //cout<<"\tleftMin = "<<leftMin<<", rightMin = "<<rightMin<<endl;
        int left = ubucket + 1, right = vbucket - 1;
        int edgeMin = h(leftMin) < h(rightMin) ? leftMin : rightMin;
        ///se nao existir buckets entre eles
        if(left > right) return edgeMin;
        ///caso contrario
        else {
            int diff = right - left + 1;
            //cout<<"\tdiferenca: "<<diff<<endl;
            int col = lg(diff);
            int minInside =
            h(table[col][left]) < h(table[col][left + diff - (1<<col)]) ?
              table[col][left]  :   table[col][left + diff - (1<<col)];
            //cout<<"\tminInside = "<<minInside<<endl;
            //cout<<endl<<endl;
            return level[index[minInside]] < level[index[edgeMin]] ? minInside : edgeMin;
        }
    }
}



int main(){

    init();
    //cout<<"DFS\n";
    dfs(0);
    build_minimum();
    build_sparse_table();

    int q, l, r;
    cin>>q;
    while(q--){
        cin>>l>>r;
        cout<<LCA(l,r)<<endl;
    }


}
