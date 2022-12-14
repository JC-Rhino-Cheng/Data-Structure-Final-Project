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

struct hope{
    int id, degree;
    bool operator <(const hope& a) const
    {
        return degree > a.degree;
    }
};
vector<set<int>> graph(VERT_NUM);
set<int> maxclique;
vector<hope> mkv;
bool map[VERT_NUM][VERT_NUM];
int maxclinum = -10000;


void signalHandler( int signum ) {
    fstream out;
    out.open("clique.txt" , ios::out);
    for(auto c: maxclique)
        out<<c<<'\n';
    exit(signum);
}

int k_core(vector<set<int>> g, vector<int> candidates, int K)
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
    flag = true;
    fstream out;
    out.open("kcore.txt" , ios::out);
    for(auto c: ans){
        out<<c.id<<" "<<c.kcore<<'\n';
        if(c.kcore == cur_k - 2){
            if(flag){
                maxclique = g[c.id];
                maxclique.insert(c.id);
                flag = false;
            }
        }
    }
    return cur_k-2;
}

bool finish = false;
int cn = 0, bestn = 0, n = 0;//cn:??????clique??????;bestn:??????clique??????;n:?????????;
bool used[VERT_NUM];//??????????????????clique???

void DFS(int i)
{
    if(finish)
        return;
    bool flag = false;
    if (i == n && !finish) //???????????????clique
    {
        bestn = cn; // ????????????clique size
        cout<<bestn<<'\n';
        maxclique.clear();
        for(auto c:mkv)
            if (used[c.id])
                maxclique.insert(c.id);
        if(bestn == 1026)
            finish = true;
        return;
    }
    for (int j = 0; j < i; j++)//???????????????????????????????????????clique
        if (used[mkv[j].id] && !map[mkv[i].id][mkv[j].id])//???????????????clique???????????????????????????????????????
        {
            flag = true;
            break;
        }
    if (!flag)
    {
        cn++;//???i?????????clique
        used[mkv[i].id] = true;
        DFS(i + 1);//??????????????????
        cn--;//??????
    }
    if (cn + n - i - 1 >= bestn) //????????????????????????
    {
        used[mkv[i].id] = false;//???i??????clique
        DFS(i + 1);//??????????????????
    }
    if(finish)
        return;
}

void generate_hope(int maxcore, vector<set<int>> g, vector<int> candidates)
{
    bool flag = true;
    while(flag){
        flag = false;
        for(auto it = candidates.begin(); it < candidates.end();){
            if(g[*it].size() < maxcore){
                for(auto c:g[*it])
                    g[c].erase(g[c].find(*it));
                candidates.erase(it);
                flag = true;
            }
            else
                it++;
        }
    }
    for(auto c:candidates)
        mkv.push_back((hope){c, (int)g[c].size()});
    sort(mkv.begin(), mkv.end());
    cout<<mkv.front().id<<" "<<mkv.front().degree<<'\n';
}

int main(int argc , char *argv[])
{
    signal(SIGINT, signalHandler);
    freopen(argv[1] , "r" , stdin);
    
    int K = stoi(argv[2]);
    
    vector<int> candidates , cur_clique;
    set<int> mem;
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
        map[A][B] = true;
        map[B][A] = true;
    }
    int maxcore = k_core(graph, candidates, K);
    generate_hope(maxcore, graph, candidates);
    n = (int)mkv.size();
    DFS(0);
    fstream out;
    out.open("clique.txt" , ios::out);
    for(auto c: maxclique)
        out<<c<<'\n';
}