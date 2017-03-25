#include "deploy.h"
#include "common.h"

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    //printInput(topo,line_num);
    Graph G;
    G.init(topo);

    //ChooseServer::testlwlp();
    ChooseServer::lpChoose();
    printf("print out serverID :\n");
    ChooseServer::printVector(ChooseServer::serverID);
    printf("print out candidate serverID :\n");
    ChooseServer::printVector(ChooseServer::serverCandidate);


}





