//
// Created by lw_co on 2017/3/25.
//

#ifndef CDN_GRAPH_H
#define CDN_GRAPH_H

#define MAXNODE 1000
#define MAXCONSUMER 500
#define MAXVALUE 1000
#define WEDEBUG


#include <string.h>
#include <stdlib.h>

/**
     * 边结点，横坐标为起始节点，纵坐标为结束节点，消费节点序号顺次增加。
     * 包含：
     * 开始节点，暂时不需要。
     * 结束节点，暂时不需要。
     * 容量/消费需求带宽
     * 单价
     * 消费节点编号，暂时不需要。
     * 未知数序号,为矩阵中从左至右，从上至下边的序号
     */
struct Arc{
    int node0;//begin
    int node1;//end
    int capacity;
    int cost;
    int id;
    Arc *next;
    Arc(): node0{-1},node1{-1},capacity(0),cost(MAXVALUE),id{-1},next{nullptr} {}
};
/**头节点
 * isWithConsumer  该节点是否连接消费节点
 * consumerNum     消费节点编号
 * require         消费需求
 * */
struct Node{
    bool isWithConsumer;
    int consumerId;
    int require;
    Arc *arc;
    Node():isWithConsumer{false},consumerId{-1},require{0},arc{nullptr} {}
};
struct Graph {
    int nodeCount;
    int arcCount;
    int consumerCount;
    int serverFee;

    Arc* gNet[MAXNODE][MAXNODE];
    /**邻接表头节点，可以查找服务节点对应消费节点*/
    Node* netNode[MAXNODE];
    /**消费节点对应服务节点*/
    int consumerNode[MAXCONSUMER];
    Graph():nodeCount{0},arcCount{0},consumerCount{0},serverFee{MAXVALUE},
            gNet{nullptr},netNode{nullptr},consumerNode{0} {}

    void init(char * topo[]);
    void gNetBuild(char * topo[]);
    void getBaseInfo(char * topo[]);
    void netNodeBuild(char *topo[]);
    void setArcId(Arc *G[][MAXNODE],int maxrow,int maxcol);
    void printGNet();
    void printNetNode();
    void printConsumerNode();
};
void Graph::init(char * topo[]) {
    getBaseInfo(topo);
    gNetBuild(topo);
    netNodeBuild(topo);
#ifdef WEDEBUG
    printGNet();
    printNetNode();
    printNetNode();
    printf("ssss");
#endif
}
void Graph::getBaseInfo(char * topo[]){
    if(topo[0]==NULL){
        printf("%s\n","input error! please check!" );
        exit(1);
    }
    char *result;
    int info[3];
    result=strtok(topo[0]," ");
    int i=0;
    while(result!=NULL){
        info[i]=atoi(result);
        result=strtok(NULL," ");
        ++i;
    }
    nodeCount=info[0];
    arcCount=info[1];
    consumerCount=info[2];
    serverFee=atoi(topo[2]);
    //测试打印
    printf("basic info: network node count, Arc count, consumer count\n");
    printf("basic info: %d,%d,%d,%d\n",nodeCount,arcCount,consumerCount,serverFee);

}
void Graph::gNetBuild(char * topo[]) {
    int i;
    char *r;
    for(i=4; i<4+arcCount; ++i){
        int tmp[4]={0};
        int k=0;
        r=strtok(topo[i]," ");
        while(r!=NULL){
            tmp[k]=atoi(r);
            r=strtok(NULL," ");
            ++k;
        }
        Arc *p=(Arc *)calloc(1,sizeof(Arc));
        p->capacity=tmp[2];
        p->cost=tmp[3];
        gNet[tmp[0] ][tmp[1] ]=p;
        //赋值另一半
        gNet[tmp[1] ][tmp[0] ]=p;

        //build twice of edge,and use the matrix edge p
        /**adjoin table,head join*/
        p->node0=tmp[0];
        p->node1=tmp[1];
        if(netNode[tmp[0]]!= nullptr){
            p->next=netNode[tmp[0] ]->arc;
            netNode[tmp[0]]->arc=p;
        }else{
            Node *d=(Node *)calloc(1,sizeof(Node));
            d->arc=p;
            netNode[tmp[0]]=d;
        }


        Arc *q=(Arc *)calloc(1,sizeof(Arc));
        memcpy(q,p,sizeof(*p));
        q->node1=tmp[0];
        q->node0=tmp[1];
        /**adjoin table,head join*/
        if(netNode[tmp[1]]!= nullptr) {
            q->next = netNode[tmp[1]]->arc;
            netNode[tmp[1]]->arc=q;
        }else{
            Node *d=(Node *)calloc(1,sizeof(Node));
            d->arc=q;
            netNode[tmp[1]]=d;
        }
    }
}
void Graph::netNodeBuild(char **topo) {
    int i;
    char *r;
    for(i=5+arcCount; i<5+arcCount+consumerCount;++i){
        int tmp[3]={0};
        int k=0;
        r=strtok(topo[i]," ");
        while(r!=NULL){
            tmp[k]=atoi(r);
            r=strtok(NULL," ");
            ++k;
        }
        Node *d=(Node *)calloc(1,sizeof(Node));
        d->require=tmp[2];
        d->isWithConsumer=true;
        d->consumerId=tmp[0];
        netNode[tmp[0]]=d;

        consumerNode[tmp[0]]=tmp[1];
    }
}
void Graph::printGNet(){
    printf("gNet printf! *********");
    for(int i=0;i<nodeCount;++i){
        for(int j=i;j<nodeCount;++j){
            if(gNet[i][j]!= nullptr){
                printf("%d->%d,%d,%d\n",
                       gNet[i][j]->node0,
                       gNet[i][j]->node1,
                       gNet[i][j]->capacity,
                       gNet[i][j]->cost);
            }
        }
    }
}
void Graph::printNetNode() {
    printf("printNetNode! *********");
    for(int i=0;i<nodeCount;++i){
        Arc *p=netNode[i]->arc;
        printf("the Node %d connect the nodes is: ",i);
        while(p!= nullptr){
            printf("%d ",p->node1);
            p=p->next;
        }
        printf("\n");
    }
}
void Graph::printConsumerNode(){
    printf("printConsumerNode ! ***********");
    for(int i=0;i<consumerCount;++i){
        printf("%d->%d",i,consumerNode[i]);
    }
}
//void Graph::getBaseInfo(char **topo) {}



#endif //CDN_GRAPH_H
