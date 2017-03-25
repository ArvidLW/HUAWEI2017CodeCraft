#include "deploy.h"
#include <stdio.h>
#include <vector>
#include "Graph.h"
#include "chooseServer.h"
#include <iostream>

//#define MODE1 //设定了超源节点变量，缺点，变量多了，不好计算
//#define MODE2 //取消了超源节点变量，并合并了f_{i,j}的双向流量，变为单向，缺点，不好确定服务器位置
#define MODE3 //取消了超源节点变量，合并到网络节点流量差约束中，info[0]+2info[1]个变量，约束为info[0]+2*info[1]个
//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    //printInput(topo,line_num);
    Graph G;
    G.init(topo);
    //ChooseServer::testlwlp();
    ChooseServer::lpChoose();

}




