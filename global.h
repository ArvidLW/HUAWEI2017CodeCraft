#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <list>
#include <string.h>
#include <bitset>
#include <stack>
#include <queue>
#include <math.h>
#include <assert.h>
#include <unordered_map>
#define INF 999999
#define MAX_EDGE_NUM (1000*1000+1)
#define MAX_NODE_NUM 1001

using namespace std;
extern int source_p;
extern int sink_p;
extern int node_num;
extern int link_num;
typedef struct Link{
    Link() : u(-1), v(-1), next(-1), cap(0), cost(INF){}
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
    stack<int> nodes;
    Path()
    {
        cost=0;
        flow=0;
        nodes(MAX_NODE_NUM,-1);
    }
};
#endif // GLOBAL_H_INCLUDED
