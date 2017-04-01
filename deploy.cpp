/*
 * Copyright 2017
 *
 * deploy.cpp
 *
 * 功能入口
 *
 * Date: March 26, 2017
 * Author: Wei Liu, Ling Bao, Shangyue Zhou
 *
 * $Id: deploy.h,v 0.1 2017/03/26 00:22:35 bl Exp $
 *
 */
#include "deploy.h"
#include "common.h"

//#define Mc
#define Zk

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    char* result;
    double minCost;
    Graph G;
    G.init(topo);
    DirectOUT::outResult();
    ChooseServer::lpChoose();//线性规划选择服务器
    ChooseServer::printServerInfo();//打印所选服务器信息

    std::vector<int> serverTmp{ChooseServer::serverID};
    serverTmp.insert(serverTmp.end(),ChooseServer::serverCandidate.begin(),ChooseServer::serverCandidate.end());
    ChooseServer::minCostCan=mc.run(Graph::nodeCount,Graph::arcCount,serverTmp);

    serverTmp.insert(serverTmp.end(),ChooseServer::serverPossible.begin(),ChooseServer::serverPossible.end());
    ChooseServer::minCostPos=mc.run(Graph::nodeCount,Graph::arcCount,serverTmp);

    ChooseServer::minCost=mc.run(Graph::nodeCount,Graph::arcCount,ChooseServer::serverID);

    printf(splitLine);
    printf("minCost: %.f, minCostCan: %.f, minCostPos: %.f\n",ChooseServer::minCost,ChooseServer::minCostCan,ChooseServer::minCostPos);

    timer.Begin();//计时开始
    WeGa wega(filename);
    wega.chooseServer();

    timer.End();//计时结束
    std::cout<<"time: "<<timer.ms()<<std::endl;
    result=mc.getRoute();
    minCost=mc.minicost;
    printf("we get minCost : %.f\n" ,minCost);
    //printf("%sminCost is : %.f\n\n%s\n",splitLine,minCost,result);
    printf(splitLine);
    write_result(result,filename);


}





