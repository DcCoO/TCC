#include <bits/stdc++.h>
using namespace std;




int main(){
    int n = 15;

    string filename = "complete" + to_string(n) + ".txt";
    freopen(filename.c_str(), "w", stdout);
    //cout<<filename<<endl;



    int totalNodes = (1<<n) - 1;
    cout<<totalNodes<<endl;
    int limit = (1<<(n-1)) - 1;

    if(n == 1) cout<<0<<endl;
    else {
        for(int i = 0; i < limit; i++) cout<<2<<" "<<(2 * i + 1)<<" "<<(2 * i + 2)<<endl;
        for(int i = 0, len = 1<<(n-1); i < len; i++) cout<<0<<endl;
    }



}
