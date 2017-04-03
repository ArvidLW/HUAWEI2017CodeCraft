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
    //DirectOUT::outResult();
    //ChooseServer::lpChoose();//线性规划选择服务器
    //ChooseServer::printServerInfo();//打印所选服务器信息
    int s[]={0,9,23,42,61,77,86,88,105,125,143,156,176,193,198,201,204,221,233,253,256,267,281,290,304,324,336,356,375,388,391,394,414,416,429,449,454,458,469,477,482,491,507,512,525,537,557,573,590,601,613,628,630,631,642,659,660,671,691,694,700,702,716,731,749,759,771,791};
    //int s[]={1,21,52,87,98,105,125,141,154,156,157,171,205,241,244,254,291,308,320,355,364,380,414,451,487,516,553,587,599,615,624,636,647,668,697,706,722,735,762,767,797};
    //int s[]={11, 13, 17, 22, 34, 49, 50, 54, 62, 63, 68, 75, 77, 81, 89, 95, 100, 102, 119, 121, 122, 127, 133, 135, 137, 141, 147, 148, 149, 150, 151, 167, 173, 183, 185, 190, 196, 204, 205, 218, 219, 221, 235, 238, 245, 246, 250, 260, 263, 270, 271, 283, 284, 289, 290, 291, 297, 304, 311, 313, 319, 335, 348, 352, 360, 372, 378, 384, 385, 393, 394, 396, 406, 410, 417, 424, 444, 450, 465, 473, 477, 478, 482, 503, 506, 530, 537, 541, 542, 571, 579, 581, 582, 585, 587, 590, 595, 597, 600, 607, 608, 615, 624, 628, 638, 645, 656, 668, 672, 682, 686, 687, 689, 691, 698, 700, 711, 721, 724, 729, 730, 732, 735, 736, 743, 762, 763, 781};
    //int s[]={13,23,55,60,76,92,96,97,127,138,152,154,170,189,190,200,224,246,268,290,320,347,352,360,392,426,436,469,504,506,539,553,561,565,576,583,608,625,646,676,706,730,759,764,769,773};
    ChooseServer::serverID.resize(sizeof(s)/sizeof(int));
    ChooseServer::serverID.assign(&s[0],&s[sizeof(s)/ sizeof(int)]);
//
    WeMCMF m{ChooseServer::serverID};

    timer.Begin();//计时开始
    m.mcmf();
    timer.End();
    printf("lw minCost: %.f, time: %.f\n",m.minCost+ChooseServer::serverID.size()*Graph::serverFee,timer.ms());
    printf("lw minCost: %.f, time: %.f\n",m.minCost,timer.ms());


//    timer.Begin();//计时开始
//    mc.run(Graph::nodeCount,Graph::arcCount,ChooseServer::serverID);
//    timer.End();
//    printf("zkw lw minCost: %.f, time: %.f\n",mc.minicost,timer.ms());
//    printf("zkw lw minCost: %.f, time: %.f\n",mc.minicost-ChooseServer::serverID.size()*Graph::serverFee,timer.ms());

//    std::vector<int> serverTmp{ChooseServer::serverID};
//    serverTmp.insert(serverTmp.end(),ChooseServer::serverCandidate.begin(),ChooseServer::serverCandidate.end());
//    ChooseServer::minCostCan=mc.run(Graph::nodeCount,Graph::arcCount,serverTmp);
//
//    serverTmp.insert(serverTmp.end(),ChooseServer::serverPossible.begin(),ChooseServer::serverPossible.end());
//    ChooseServer::minCostPos=mc.run(Graph::nodeCount,Graph::arcCount,serverTmp);
//
//    ChooseServer::minCost=mc.run(Graph::nodeCount,Graph::arcCount,ChooseServer::serverID);
//
//    printf(splitLine);
//    printf("minCost: %.f, minCostCan: %.f, minCostPos: %.f\n",ChooseServer::minCost,ChooseServer::minCostCan,ChooseServer::minCostPos);
//
//    timer.Begin();//计时开始
//    WeGa wega(filename);
//    wega.chooseServer();
//
//    timer.End();//计时结束
//    std::cout<<"time: "<<timer.ms()<<std::endl;
//    result=mc.getRoute();
//    minCost=mc.minicost;
//    printf("we get minCost : %.f\n" ,minCost);
//    //printf("%sminCost is : %.f\n\n%s\n",splitLine,minCost,result);
//    printf(splitLine);
//    write_result(result,filename);


}





