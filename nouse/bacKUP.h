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
    double minCost{0};
    Graph G;
    G.init(topo);
    //DirectOUT::outResult();
    ChooseServer::lpChoose();//线性规划选择服务器
    //ChooseServer::printServerInfo();//打印所选服务器信息

/**--------------------------TEST-----------------------------------**/
//    //int s[]={0,9,23,42,61,77,86,88,105,125,143,156,176,193,198,201,204,221,233,253,256,267,281,290,304,324,336,356,375,388,391,394,414,416,429,449,454,458,469,477,482,491,507,512,525,537,557,573,590,601,613,628,630,631,642,659,660,671,691,694,700,702,716,731,749,759,771,791};
//    //int s[]={1,21,52,87,98,105,125,141,154,156,157,171,205,241,244,254,291,308,320,355,364,380,414,451,487,516,553,587,599,615,624,636,647,668,697,706,722,735,762,767,797};
//    //int s[]={11, 13, 17, 22, 34, 49, 50, 54, 62, 63, 68, 75, 77, 81, 89, 95, 100, 102, 119, 121, 122, 127, 133, 135, 137, 141, 147, 148, 149, 150, 151, 167, 173, 183, 185, 190, 196, 204, 205, 218, 219, 221, 235, 238, 245, 246, 250, 260, 263, 270, 271, 283, 284, 289, 290, 291, 297, 304, 311, 313, 319, 335, 348, 352, 360, 372, 378, 384, 385, 393, 394, 396, 406, 410, 417, 424, 444, 450, 465, 473, 477, 478, 482, 503, 506, 530, 537, 541, 542, 571, 579, 581, 582, 585, 587, 590, 595, 597, 600, 607, 608, 615, 624, 628, 638, 645, 656, 668, 672, 682, 686, 687, 689, 691, 698, 700, 711, 721, 724, 729, 730, 732, 735, 736, 743, 762, 763, 781};
    //int s[]={13,23,55,60,76,92,96,97,127,138,152,154,170,189,190,200,224,246,268,290,320,347,352,360,392,426,436,469,504,506,539,553,561,565,576,583,608,625,646,676,706,730,759,764,769,773};

//    int s[]={271 ,585 ,167 ,290 ,121 ,571 ,133 ,75 ,68 ,148 ,537 ,77 ,530 ,348 ,205 ,672 ,597 ,137 ,49 ,50 ,590 ,246 ,218 ,582 ,595 ,
//         291 ,624 ,781 ,682 ,289 ,396 ,22 ,656 ,141 ,763 ,150 ,352 ,11 ,221 ,743 ,173 ,185 ,721 ,700 ,608 ,691 ,34 ,406 ,735 ,62
//        ,587 ,149 ,473 ,147 ,628 ,219 ,183 ,270 ,410 ,385 ,478 ,724 ,668 ,581 ,360 ,283 ,732 ,711 ,372 ,378 ,245 ,260 ,482 ,736
//        ,686 ,417 ,384 ,394 ,81 ,36 ,207 ,390 ,658 };
//    std::vector<int> serverT;
//    serverT.resize(sizeof(s)/sizeof(int));
//    serverT.assign(&s[0],&s[sizeof(s)/ sizeof(int)]);
//
    //WeMCMF m;
//    minCost=m.run(Graph::nodeCount,Graph::arcCount,serverT);

/**----------------------------------------------------------------**/
/**--------------------------WeGa-----------------------------------**/
    std::vector<int> serverTmp{ChooseServer::serverID};
    //serverTmp.insert(serverTmp.end(),ChooseServer::serverCandidate.begin(),ChooseServer::serverCandidate.end());
    //ChooseServer::minCostCan=mc.run(Graph::nodeCount,Graph::arcCount,serverTmp);

    //serverTmp.insert(serverTmp.end(),ChooseServer::serverPossible.begin(),ChooseServer::serverPossible.end());
    //ChooseServer::minCostPos=mc.run(Graph::nodeCount,Graph::arcCount,serverTmp);

    //ChooseServer::minCost=mc.run(Graph::nodeCount,Graph::arcCount,ChooseServer::serverID);

    //86847,case_x2.txt
    std::string strs{"11111111111111111111111111111111111101111111110000010001000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"};
    std::vector<int> serverT1;
    for (int i = 0; i <strs.size() ; ++i) {
        if(strs.at(i)=='1'){
            serverT1.push_back(serverTmp[i]);
        }
    }
//
    WeMCMF m;
    minCost=m.run(Graph::nodeCount,Graph::arcCount,serverT1);
    m.clearData();

    WeMCMF1 m1;
    m1.run(Graph::nodeCount,Graph::arcCount,serverT1);
    m1.getRoute();
    m1.clearData(m1.s);
    //ZKW zkw1;
    //zkw1.run(Graph::nodeCount,Graph::arcCount,serverT1);
//    printf(splitLine);
//    printf("minCost: %.f, minCostCan: %.f, minCostPos: %.f\n",ChooseServer::minCost,ChooseServer::minCostCan,ChooseServer::minCostPos);
//
//    timer.Begin();//计时开始
//    WeGa wega(filename);
//    wega.chooseServer();
/**-------------------------------------------------------------**/
/**---------------------------Ga----------------------------------**/
//    OurGA ourGA = OurGA(filename);
//    if (ourGA.bSolve) {
//        ourGA.GaAlgorithmServer();
//    }
/**-------------------------------------------------------------**/

    //printf(splitLine);
    printf("we get minCost : %.f\n" ,minCost);
    printf("%s\n",m1.result);
    //printf("%s\n",m.s);
    printf(splitLine);

    //write_result(m1.result,filename);


}





