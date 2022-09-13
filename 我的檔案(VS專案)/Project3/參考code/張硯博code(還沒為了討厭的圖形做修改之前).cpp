#include <vector>
#include <csignal>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <signal.h>
#include <set>
#define VERT_NUM  82168
using namespace std;

struct vertex{
    int id, kcore;
    bool operator <(const vertex& a) const
    {
        return id < a.id;
    }
};

vector<set<int>> graph(VERT_NUM);

void signalHandler( int signum ) {
    /*
     In the signal handler, we output the current best clique that we found.
     */
    exit(signum);
}

bool k_core(vector<set<int>> g, vector<int> candidates, int K, bool f)
{
    bool flag = true;
    while(flag){
        flag = false;
        for(auto it = candidates.begin(); it < candidates.end();){
            if(g[*it].size() < K){
                for(auto c:g[*it])
                    g[c].erase(g[c].find(*it));
                candidates.erase(it);
                flag = true;
            }
            else
                it++;
        }
    }
    int cur_k = K + 1;
    vector<vertex> ans;
    while(candidates.size()){
        flag = true;
        while(flag){
            flag = false;
            for(auto it = candidates.begin(); it < candidates.end();){
                if(g[*it].size() < cur_k){
                    for(auto c:g[*it])
                        g[c].erase(g[c].find(*it));
                    ans.push_back((vertex){*it, cur_k-1});
                    candidates.erase(it);
                    flag = true;
                }
                else
                    it++;
            }
        }
        cur_k++;
    }
    sort(ans.begin(), ans.end());
    fstream out, out2;
    out.open("kcore.txt" , ios::out);
    out2.open("clique.txt" , ios::out);
    if(ans.size() != 0){
        flag = true;
        for(auto c: ans){
            if(f)
                out<<c.id<<" "<<c.kcore<<'\n';
            if(c.kcore == cur_k-2 && flag){
                out2<<c.id<<'\n';
                for(auto d: g[c.id])
                    out2<<d<<'\n';
                flag = false;
                if(!f)
                    break;
            }
        }
        return 1;
    }
    else
        return 0;
}


int main(int argc , char *argv[])
{
    
    signal(SIGINT, signalHandler);
    freopen(argv[1] , "r" , stdin);
    
    int K = stoi(argv[2]);
    
    vector<int> candidates , cur_clique;
    for(int i = 0 ; i < VERT_NUM ; i++){
        candidates.push_back(i);
    }
    int A , B;
    while(cin >> A >> B){
        if(A == B){
            continue;
        }
        graph[A].insert(B);
        graph[B].insert(A);
    }
    if(!k_core(graph, candidates, K, 1))
        k_core(graph, candidates, 50, 0);
}