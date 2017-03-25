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

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    Graph G;
    G.init(topo);

    //ChooseServer::testlwlp();
    ChooseServer::lpChoose();
    printf("print out serverID :\n");
    ChooseServer::printVector(ChooseServer::serverID);
    printf("print out candidate serverID :\n");
    ChooseServer::printVector(ChooseServer::serverCandidate);

}





