//
// Created by lw_co on 2017/3/20.
//

#ifndef CDN_INITIALDATA_H
#define CDN_INITIALDATA_H

#define MAXNODE 1000
#define MAXCONSUMER 500

#include <string.h>


/**
 * 边结点，横坐标为起始节点，纵坐标为结束节点，消费节点序号顺次增加。
 * 包含：
 * 开始节点，暂时不需要。
 * 结束节点，暂时不需要。
 * 容量/消费需求带宽
 * 单价
 * 结束节点是否为消费节点
 * 消费节点编号，暂时不需要。
 * 未知数序号,为矩阵中从左至右，从上至下边的序号
 */
struct MyArc
{
    //int begin;
    //int end;
    int capacity;
    int unitcost;
    bool iswithconsumer;
    int n;
    //int consumerId;
    MyArc(): capacity(0),unitcost(0),iswithconsumer(false){}
};

/**
 * *边存储
 * 图信息info：网络节点、网络节点链路数（边数）、消费节点、服务器费用
 * 矩阵gNet是对称矩阵，cNet是对称矩阵。
 *
 * gNet:二维指针数组，坐标为网络节点编号，为对称矩阵，值为Arc *，指向相应边。
 * CNet:二维指针数组，模坐标为消费节点编号，纵坐标网络节点编号，值为Arc *，指向相应边
 *
 * *节点对应存储，方便找到消费节点连接的网络节点，以及网络节点连接的消费节点。
 * consumerNode 存放消费节点连接的网络节点
 * netNode 存放网络节点连接的消费节点和带宽,索引为网络节点编号
 *      netNode[0][i]为网络节点i对应的消费节点
 *      netNode[1][i]为网络节点i对应的消费节点需求。
 *
 *
 */
int info[4]={0};
MyArc *gNet[MAXNODE][MAXNODE]={nullptr};
MyArc *cNet[MAXCONSUMER][MAXNODE]={nullptr};

int consumerNode[MAXCONSUMER]={0};
int netNode[2][MAXNODE]={0};
/**
 * serverID存放服务器节点编号
 * */
std::vector<int> serverID;

/**
 * 功能总入口
 * 提取信息info
 * 建立数组gNet、cNet
 */
void printInput(char * topo[MAX_EDGE_NUM], int line_num);
/**
 * getInfo
 * 分割topo[0]中信息存入info;
 */
void getInfo(char * topo[MAX_EDGE_NUM]);
/**
 * cNetBuild
 * 建立消费节点与网络节点数组cNet。
 */
void cNetBuild(char * topo[MAX_EDGE_NUM]);
/**
 * gNetBuild
 * 建立网络结点数组gNet
 */
void gNetBuild(char * topo[MAX_EDGE_NUM]);
/**
 * printArc
 * 打印cNet或者gNet
 */
void printArc(MyArc *G[][MAXNODE],int maxrow,int maxcol);
/**
 * mylpsolve
 * 整数规划
 * @return [成功or失败]
 */

int mylpsolve(void);
/**
 * setArcId
 * 设置在线性规划中，边的编号，按上三角的顺序，按照遇到的边的顺序递增编号，边编号从0开始
 * */
void setArcId(MyArc *G[][MAXNODE],int maxrow,int maxcol);
void joinArr(double c[], int sc,double a[5],int sa, double b[], int sb);
void printArr(double a[],int m);



/*****方法实现************************************************/

void setArcId(MyArc *G[][MAXNODE],int maxrow,int maxcol){
    int i,j,k;
    printf("\n network node link info\n");
    for(i=0,k=0; i<maxrow; ++i){
        //注意由于给的无向图，所以一半就行
        for(j=i; j<maxcol; ++j){
            if(G[i][j]!=NULL){
                G[i][j]->n=k;
                ++k;
            }
        }
    }
}
void printArc(MyArc *G[][MAXNODE],int maxrow,int maxcol){
    int i,j;
    printf("\nnetwork node link info\n");
    for(i=0; i<maxrow; ++i){
        //注意由于给的无向图，所以打印一半就行
        for(j=i; j<maxcol; ++j){
            if(G[i][j]!=NULL){
                printf("%d->%d, %d, %d,iswithconsumer:%d,variable serial number:%d\n",i,j,G[i][j]->capacity,G[i][j]->unitcost,G[i][j]->iswithconsumer,G[i][j]->n);
            }
        }
    }
}

void getInfo(char * topo[MAX_EDGE_NUM]){
    if(topo[0]==NULL){
        printf("%s\n","input error! please check!" );
        exit(1);
    }
    char *result;
    result=strtok(topo[0]," ");
    int i=0;
    while(result!=NULL){
        info[i]=atoi(result);
        result=strtok(NULL," ");
        ++i;
    }

    info[i]=atoi(topo[2]);
    //测试打印
    printf("basic info: network node count, Arc count, consumer count\n");
    for(i=0;i<4;++i){
        printf("%d ",info[i] );
    }
    printf("\n");
}

void printInput(char * topo[MAX_EDGE_NUM], int line_num){
    int i;
    for(i=0;i<line_num;++i){
        printf("%s", topo[i] );
    }
}

void cNetBuild(char * topo[MAX_EDGE_NUM]){
    int i;
    char *r;
    for(i=5+info[1]; i<5+info[1]+info[2];++i){
        int tmp[3]={0};
        int k=0;
        r=strtok(topo[i]," ");
        while(r!=NULL){
            tmp[k]=atoi(r);
            r=strtok(NULL," ");
            ++k;
        }
        MyArc *p=(MyArc *)calloc(1,sizeof(MyArc));
        p->capacity=tmp[2];
        p->iswithconsumer=true;
        cNet[tmp[0] ][tmp[1] ]=p;
        //赋值另一半
        //printf("A::%d,,,,,,%d\n",tmp[0],tmp[1]);
        //cNet[tmp[1] ][tmp[0] ]=p;
        //printf("B::%d,,,,,,%d\n",tmp[0],tmp[1]);

        consumerNode[tmp[0] ]=tmp[1];
        netNode[0][tmp[1] ]=tmp[0];
        //赋值消费需求
        netNode[1][tmp[1] ]=tmp[2];
    }
}

void gNetBuild(char * topo[MAX_EDGE_NUM]){
    int i;
    char *r;
    for(i=4; i<4+info[1]; ++i){
        int tmp[4]={0};
        int k=0;
        r=strtok(topo[i]," ");
        while(r!=NULL){
            tmp[k]=atoi(r);
            r=strtok(NULL," ");
            ++k;
        }
        MyArc *p=(MyArc *)calloc(1,sizeof(MyArc));
        p->capacity=tmp[2];
        p->unitcost=tmp[3];
        gNet[tmp[0] ][tmp[1] ]=p;
        //赋值另一半
        gNet[tmp[1] ][tmp[0] ]=p;
    }

}
/**
 * 最后个数组拼接了两次，由于边的双向性，所以一条边是两个变量，但单价一样。
 */
void joinArr(double c[], int sc,double a[5],int sa, double b[], int sb){
    int i,j;
    //形参不能计算数组大小，因为是指针
    for(i=0,j=0;i<sa;++i,++j){
        c[j]=a[i];
    }
    for(i=0;i<sb;++i,++j){
        c[j]=b[i];
    }
    for(i=0;i<sb;++i,++j){
        c[j]=b[i];
    }
    /*for(i=0;i<sc;++i){
        printf("%.0f, ",c[i]);
    }
    printf("看看sc=%d,sa=%d,sb=%d\n",sc,sa,sb);
    */
}
//void joinArr(double c[], int sc,double a[],int sa, double b[], int sb){
//    int i,j;
//    for(i=0;i<info[0];++i){
//        c[i]=-DOUBLE_MAX;
//    }
//    //形参不能计算数组大小，因为是指针
//    for(i=0,j=3*info[0]+1;i<sa;++i,++j){
//        c[j]=a[i];
//    }
//    for(i=0;i<sb;++i,++j){
//        c[j]=b[i];
//    }
//    for(i=0;i<sb;++i,++j){
//        c[j]=b[i];
//    }
//    c[5*info[0]+2*info[1]+1]=-DOUBLE_MAX;
//    /*for(i=0;i<sc;++i){
//        printf("%.0f, ",c[i]);
//    }
//    printf("看看sc=%d,sa=%d,sb=%d\n",sc,sa,sb);
//    */
//}

void printArr(double a[],int m){
    int i;
    for(i=0;i<m;++i){
        printf("%.0f ",a[i] );
    }
    printf("\n");
}


#endif //CDN_INITIALDATA_H
