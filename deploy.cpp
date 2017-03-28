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
    Graph G;
    G.init(topo);
    //DirectOUT::outResult();//直接设置消费节点相连的网络节点为服务器，并输出结果
    ChooseServer::lpChoose();//线性规划选择服务器
    printf("print out serverID :\n");
    ChooseServer::printVector(ChooseServer::serverID);//打印服务器节点
    printf("print out candidate serverID :\n");
    ChooseServer::printVector(ChooseServer::serverCandidate);//打印备选服务器节点

//    OurGA ourGA = OurGA();
//    ourGA.GaAlgorithmServer(ChooseServer::serverID, ChooseServer::serverCandidate, Graph::nodeCount, Graph::arcCount, filename);

    timer.Begin();//计时开始
#ifdef Mc
    MCMF m;
    result=m.run(Graph::nodeCount,Graph::arcCount);
#endif // Mc

#ifdef Zk
    ZKW z;
    result=z.run(Graph::nodeCount,Graph::arcCount);
#endif // Zk
    timer.End();//计时结束
    std::cout<<"timer:"<<timer.ms()<<std::endl;
    //write_result(result,filename);
    //write_result(mcmf.s,filename);

}





