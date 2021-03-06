/*
 * Copyright 2017
 *
 * WeMCMF.h
 *
 * 最小费用，最大流
 *
 * Date: March 26, 2017
 * Author: Wei Liu, Ling Bao, Shangyue Zhou
 *
 * 思想：
 *
 * 在计算最小费用最大流时，要注意设置反向边，否则，费用会偏大
 *
 * 这里用两条有向边，包含两个容量，一个正向一个逆向
 *
 * c(a,b)=f(a,b)+f(b,a)
 *
 * 注意其中细节
 *
 * 优化，SLF,LLL
 *
 * 每次spfa在以前基础上计算
 *
 * $Id: WeMCMF.h,v 0.1 2017/03/26 00:30:35 lw Exp $
 *
 */

#ifndef CDN_WEMCMF_H
#define CDN_WEMCMF_H


struct WeMCMF {

    double mCost[MAXNODE];
    int pre[MAXNODE];
    std::bitset<MAXNODE> isInQue;
    std::deque<Arc*> que;
    double minCost;
    double sumFlow;//use for check
    int s;//super source
    int t;//super sink

    void addSource(std::vector<int> &server);
    void addSink();
    bool spfa();
    void mcmf();
    WeMCMF(std::vector<int> &server):minCost{0},sumFlow{0},s{1000},t{1001} {

        addSource(server);
        addSink();
    }
};
void WeMCMF::mcmf() {
    memset(mCost,0x70, sizeof(mCost));
    memset(pre,-1,sizeof(pre));
    double minFlow;
    int minFlowLoc;
    int u=s;
    mCost[s]=0;
    pre[s]=s;
    isInQue.set(s);
    que.push_back(Graph::netNode[s]->arc);
    while(spfa()){
        minFlow=INF;
        for(int i=t;i!=s;i=pre[i]){
            printf("is here,%d,%d\n",i,pre[i]);
            //比较改为小于等于，方便找链路最前面的那个通过量最小的
            if(Graph::gNet[pre[i] ][i]->rCapacity > 0){
                if(minFlow >= Graph::gNet[pre[i] ][i]->rCapacity){
                    minFlow = Graph::gNet[pre[i] ][i]->rCapacity;
                    minFlowLoc=pre[i];
                }
            }
            else{
                if(minFlow >= Graph::gNet[pre[i] ][i]->mCapacity ){
                    minFlow = Graph::gNet[pre[i] ][i]->mCapacity;
                    minFlowLoc=pre[i];
                }
            }

        }
        sumFlow=sumFlow+minFlow;
        for(int i=t;i!=s;i=pre[i]){
            //printf("minFlow= %.f, capacity= %.f\n",minFlow,Graph::gNet[pre[i] ][i]->rCapacity);
            if(Graph::gNet[pre[i] ][i]->rCapacity > 0){
                Graph::gNet[pre[i] ][i]->rCapacity=Graph::gNet[pre[i] ][i]->rCapacity - minFlow;
                minCost=minCost - minFlow*Graph::gNet[pre[i] ][i]->cost;

                if(i!=t&&pre[i]!=s){
                    Graph::gNet[i ][pre[i]]->mCapacity=Graph::gNet[i ][pre[i]]->capacity - Graph::gNet[pre[i] ][i]->rCapacity;

                }
                //printf("-1 %d -> %d ,minFlow= %.f, capacity= %.f\n",pre[i],i,minFlow,Graph::gNet[pre[i] ][i]->rCapacity);
            }
            else{
                Graph::gNet[pre[i] ][i]->mCapacity = Graph::gNet[pre[i] ][i]->mCapacity -minFlow;
                minCost=minCost + minFlow*Graph::gNet[pre[i] ][i]->cost;
                if(i!=t && pre[i]!=s){
                    Graph::gNet[i ][pre[i]]->rCapacity=Graph::gNet[i ][pre[i]]->capacity - Graph::gNet[pre[i] ][i]->mCapacity;
                }
                //printf("+1 %d -> %d ,minFlow= %.f, capacity= %.f\n",pre[i],i,minFlow,Graph::gNet[pre[i] ][i]->mCapacity);
            }
        }


        //int cc=0;
        std::queue<int> vQue;
        vQue.push(minFlowLoc);
        int vt;
        while(!vQue.empty()){
            vt=vQue.front();
            vQue.pop();
            Arc *p=Graph::netNode[vt]->arc;
            while(p!= nullptr){
                if(pre[p->node1] == vt){
                    pre[p->node1] = -1;
                    mCost[p->node1] = INF;
                    vQue.push(p->node1);
                    //++cc;
                }
                p=p->next;
            }
        }


        //(3) 把距离变为很大的节点的相临的点,距离最短的推入，其节点前驱不为-1，思想是没有错的
//        for(int i=0;i<Graph::nodeCount;++i){
//            if(pre[i]==-1){
//                //把距离他最小的点，且该点有前驱，推入
//                Arc *q=Graph::netNode[i]->arc;
//                int mTmp=INF+INF;
//                int mLoc=-1;
//                while(q!= nullptr){
//                    //有可能有反向边，所以不加mTmp>mCost[q->node1]+Graph::gNet[q->node1][q->node0]->cost 这个判断
//                    if(!isInQue[q->node1] && pre[q->node1]!=-1 && q->node1!=t && Graph::gNet[q->node1][q->node0]->mCapacity>0){
//                        //查看谁离该点更近，记录下来
//                        if(Graph::gNet[q->node1][q->node0]->rCapacity > 0){
//                            if(mTmp>mCost[q->node1] - q->cost){
//                                mTmp=mCost[q->node1] - q->cost;
//                                mLoc=q->node1;
//                            }
//                        }
//                        else{
//                            if(mTmp>mCost[q->node1] + q->cost){
//                                mTmp=mCost[q->node1] + q->cost;
//                                mLoc=q->node1;
//                            }
//                        }
//                    }
//                    //将离得最近的推进去
//                    if(mLoc!=-1){
//                        que.push_back(Graph::netNode[mLoc ]->arc);
//                        isInQue.set(mLoc );
//                    }
//                    q=q->next;
//                }
//
//            }
//        }
        //把消费节点连接的网络节点pre,mCost重置
//        for(int i=0;i<Graph::consumerCount;++i){
//            pre[Graph::consumerNode[i] ]=-1;
//            mCost[Graph::consumerNode[i] ]=INF;
//        }
        //测试

        //把为-1的节点的前驱节点推进去
//        for(int i=0;i<Graph::nodeCount;++i){
//            if(pre[i ]!=-1){
//                continue;
//            }
//            Arc *p=Graph::netNode[i ]->arc;
//            while(p){
//                if(pre[p->node1]!=-1){
//                    que.push_back(Graph::netNode[p->node1]->arc);
//                    isInQue.set(p->node1 );
//                }
//                p=p->next;
//            }
//        }

        //把消费节点连接的网络节点前驱也为-1,如果不为-1的话可能找不到解，spfa无法推进，但可能出现循环
//        for(int i=0;i<Graph::consumerCount/2;++i){
//            printf("mCap= %.f\n",Graph::gNet[Graph::consumerNode[i] ][t]->mCapacity);
//            if(Graph::gNet[Graph::consumerNode[i] ][t]->mCapacity>0){
//                pre[Graph::consumerNode[i] ]=-1;
//                mCost[Graph::consumerNode[i] ]=INF;
//            }
//
//        }
        //把为-1的消费节点连接的网络节点的前驱推进行
//        for(int i=0;i<Graph::consumerCount;++i){
//            if(pre[Graph::consumerNode[i] ]!=-1){
//                continue;
//            }
//            Arc *p=Graph::netNode[Graph::consumerNode[i] ]->arc;
//            while(p){
//                if(pre[p->node1]!=-1){
//                    que.push_back(Graph::netNode[p->node1]->arc);
//                    isInQue.set(p->node1 );
//                }
//                p=p->next;
//            }
//        }

        //(5),注意把消费节点连接的网络节点的前驱节点，推入，把消费节点连接的网络节点pre,mCost重置
//        for(int i=0;i<Graph::consumerCount;++i){
//            //消费节点连接的网络节点，的前驱节点
//            //每个只推最小的
//            if(pre[Graph::consumerNode[i] ]!=-1){
//                continue;
//            }
//            Arc *p=Graph::netNode[Graph::consumerNode[i] ]->arc;
//            int minTmp=INF+INF;
//            int minLoc=-1;
//            //printf("enter nuull i=%d\n", i);
//            //p不为空，且p还有容量,还有汇点到超汇的容量，如果满足了就ok
//            while(p!= nullptr){
//
//                //查看谁离该点更近，记录下来
//                if(!isInQue[p->node1]&& p->node1!=t  && Graph::gNet[p->node0][p->node1]->mCapacity>0 && Graph::gNet[p->node1][p->node0]->mCapacity>0){
//                    if(Graph::gNet[p->node1][p->node0]->rCapacity > 0){
//                        if(minTmp>mCost[p->node1] - p->cost){
//                            minTmp=mCost[p->node1] - p->cost;
//                            minLoc=p->node1;
//                        }
//                    }
//                    else{
//                        if(minTmp>mCost[p->node1] + p->cost){
//                            minTmp=mCost[p->node1] + p->cost;
//                            minLoc=p->node1;
//                        }
//                    }
//                }
//                p=p->next;
//            }
//            //将离得最近的推进去
//            if(minLoc!=-1){
//                que.push_back(Graph::netNode[minLoc ]->arc);
//                isInQue.set(minLoc );
//            }
//
//        }

        //(4) best
//        for(int i=0;i<Graph::nodeCount;++i){
//
//            if(pre[i]!=-1 && !isInQue[i]){
//                que.push_back(Graph::netNode[i]->arc);
//                isInQue.set(i);
//                //++cc;
//            }
//        }


//        //(3) 把距离变为很大的节点的相临的点推入，其节点前驱不为-1，思想是没有错的
        //int kk=0;

//        for(int i=0;i<Graph::nodeCount;++i){
//            if(pre[i]==-1){
//                Arc *p=Graph::netNode[i]->arc;
//
//                while (p){
//
//                    if(Graph::netNode[p->node1]->arc!= nullptr ){
//                        if(pre[p->node1]!=-1 && !isInQue[p->node1]){
//                            que.push_back(Graph::netNode[p->node1]->arc);
//                            isInQue.set(p->node1);
//                            ++kk;
//                        }
//
//                    }
//
//                    p=p->next;
//                }
//
//            }
//        }
//        printf("cc = %d, kk=%d\n",cc,kk);
        //(2)
//        for(int i=0;i<Graph::nodeCount;++i){
//            if(pre[i]!=-1 && !isInQue[i]){
//                Arc *p=Graph::netNode[i]->arc;
//                while(p){
//                    if(pre[p->node1]!=p->node0){
//                        que.push_back(Graph::netNode[i]->arc);
//                        isInQue.set(i);
//                        break;
//                    }
//                    p=p->next;
//                }
//            }
//        }
        //(1)
        //push in
        //u=minFlowLoc;
//        for(int i=0;i<Graph::nodeCount;++i){
//            if(pre[i]!=-1 && !isInQue[i]){
//                if(!que.empty()){
//                    if(mCost[i]<mCost[que.front()->node0]){
//                        que.push_front(Graph::netNode[i]->arc);
//                    } else{
//                        que.push_back(Graph::netNode[i]->arc);
//                    }
//                } else{
//                    que.push_back(Graph::netNode[i]->arc);
//                }
//
//                isInQue.set(i);
//            }
//        }

//        while(pre[u]!=s){
//            que.push_back(Graph::netNode[pre[u]]->arc);
//            isInQue.set(pre[u]);
//            vQue.push(pre[u]);
//            u=pre[u];
//        }
//        while(!vQue.empty()){
//            vt=vQue.front();
//            vQue.pop();
//            Arc *p=Graph::netNode[vt]->arc;
//            while(p!= nullptr){
//                if(pre[p->node1] == vt){
//                    que.push_back(Graph::netNode[p->node1]->arc);
//                    isInQue.set(p->node1);
//                    vQue.push(p->node1);
//                }
//                p=p->next;
//            }
//
//        }
    }
    //当流量充满时一些点不能经过，作如下处理，将该点后面受影响的值改为INF

    //然后设置一个点为起动点供spfa,该点为此次寻找链路的minFlowLoc的后继节点，的前

}
bool WeMCMF::spfa() {
//    memset(mCost,0x70, sizeof(mCost));
//    memset(pre,-1,sizeof(pre));
//    mCost[u]=0;
//    pre[u]=s;
//    isInQue.set(u);
//    que.push_back(Graph::netNode[u]->arc);
    Arc *p,*q;
    int dir=1;
    //int sum=mCost[s];
    while(!que.empty()){
        p=que.front();
        que.pop_front();
//        if(mCost[p->node0]*que.size()>sum){
//            que.push_back(p);
//            continue;
//        }
//        sum=sum-mCost[p->node0];
        printf("++++++++++++++++++++++++++++++++++enter spfa while pullptr %d->%d :%.f,  %.f,  %.f, %d\n",p->node0,p->node1,p->cost,mCost[p->node0],mCost[p->node1], dir);

        int node0=p->node0;
        while(p!= nullptr){
            if(p->rCapacity>0){
                dir=-1;
            }else{
                dir=1;
                if(p->mCapacity<=0){
                    p=p->next;
                    continue;
                }
            }
            if(mCost[p->node1] > p->cost*dir+mCost[p->node0]){
                //printf("enter spfa while pullptr %d->%d :%.f,  %.f,  %.f, %d\n",p->node0,p->node1,p->cost,mCost[p->node0],mCost[p->node1], dir);

                mCost[p->node1] = p->cost*dir+mCost[p->node0];
                if(!isInQue[p->node1]){
                    isInQue.set(p->node1);
                    if(Graph::netNode[p->node1]->arc!= nullptr){
                        if(!que.empty()){
                            //stl
                            if(mCost[p->node1]<mCost[que.front()->node0]){
                                que.push_front(Graph::netNode[p->node1]->arc);
                            }
                            else{
                                que.push_back(Graph::netNode[p->node1]->arc);
                            }
                        }
                        else{
                            que.push_back(Graph::netNode[p->node1]->arc);
                        }
                    }
                }
                //sum=sum+mCost[p->node1];
                pre[p->node1]=p->node0;
            }
            p=p->next;
        }
        isInQue.reset(node0);
    }
    return pre[t]!=-1;
}
void WeMCMF::addSource(std::vector<int> &server) {
    Graph::netNode[s]=(Node *)calloc(1, sizeof(Node));//superSource,outDegree=server.size();
    Arc *q;
    Arc tmp;
    q=&tmp;
    for (int i = 0; i < server.size(); ++i) {
        Arc *p=(Arc *)calloc(1,sizeof(Arc));
        p->node0=s;
        p->node1=server[i];
        p->capacity=INF;
        p->mCapacity=INF;
        p->cost=0;
        q->next=p;
        q=q->next;
        Graph::gNet[p->node0][p->node1]=p;
    }
    Graph::netNode[s]->arc=tmp.next;

}
void WeMCMF::addSink() {
    Graph::netNode[t]=(Node *)calloc(1, sizeof(Node));//superSink,outDegree=0;
    Arc *q;
    Arc tmp;
    q=&tmp;
    for(int i=0;i<Graph::consumerCount;++i){
        Arc *p=(Arc *)calloc(1,sizeof(Arc));
        p->node0=Graph::consumerNode[i];
        p->node1=t;
        p->capacity=Graph::netNode[Graph::consumerNode[i]]->require;
        p->mCapacity=Graph::netNode[Graph::consumerNode[i]]->require;
        p->cost=0;

        p->next=Graph::netNode[Graph::consumerNode[i]]->arc;
        Graph::netNode[Graph::consumerNode[i]]->arc=p;
        Graph::gNet[p->node0][p->node1]=p;
    }
}


#endif //CDN_WEMCMF_H
