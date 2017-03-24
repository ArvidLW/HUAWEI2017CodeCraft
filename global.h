#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <list>
#include <string.h>
#include <string>
#include <bitset>
#include <stack>
#include <queue>
#include <math.h>
#include <assert.h>
#include <unordered_map>
#define INF 9999
#define MAX_EDGE_NUM (2000*20+1500)
#define MAX_NODE_NUM 1010

using namespace std;

typedef struct Link{
    Link() : u(-1), v(-1), cap(0), cost(INF), next(-1){}
    int u;
    int v;
    int cap;
    int cost;
    int next;
};//记录边

typedef struct PRE{
    int n;
    int e;
    PRE(){
        n=-1;
        e=-1;
    };
};//记录前驱点以及和前驱点构成的边号

typedef struct Path{
    int cost;
    int flow;
    deque<int> nodes;
    Path()
    {
        cost=0;
        flow=0;
        nodes.clear();
    }
    void Print()
    {
        cout<<"Flow: "<<flow<<" Cost: "<<cost<<endl;
        for(int p:nodes)
        {
            cout<<p<<"-";
        }
        cout<<endl;
    }
};
#endif // GLOBAL_H_INCLUDED
