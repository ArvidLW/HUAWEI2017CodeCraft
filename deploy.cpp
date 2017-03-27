/*
 * Copyright 2017
 *
 * deploy.h
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

#define Mc
//#define Zk

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    char* result;
    Graph G;
    G.init(topo);
    //ChooseServer::testlwlp();
    DirectOUT::outResult();
    ChooseServer::lpChoose();
    printf("print out serverID :\n");
    ChooseServer::printVector(ChooseServer::serverID);
    printf("print out candidate serverID :\n");
    ChooseServer::printVector(ChooseServer::serverCandidate);

    //OurGA ourGA = OurGA();
    //ourGA.GaAlgorithmServer(ChooseServer::serverID, ChooseServer::serverCandidate, Graph::nodeCount, Graph::arcCount, filename);
#ifdef Mc
    MCMF m;
    result=m.run(Graph::nodeCount,Graph::arcCount);
#endif // Mc

#ifdef Zk
    ZKW z;
    result=z.run(Graph::nodeCount,Graph::arcCount);
#endif // Zk
    //write_result(result,filename);
    //write_result(mcmf.s,filename);

}





