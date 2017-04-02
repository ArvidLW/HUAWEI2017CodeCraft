//
// Created by lw_co on 2017/4/1.
//

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
    double minFlow;
    while(spfa()){
        minFlow=INF;
        for(int i=t;i!=s;i=pre[i]){
            if(Graph::gNet[pre[i] ][i]->rCapacity > 0){
                if(minFlow > Graph::gNet[pre[i] ][i]->rCapacity){
                    minFlow = Graph::gNet[pre[i] ][i]->rCapacity;
                }
            }
            else{
                if(minFlow > Graph::gNet[pre[i] ][i]->mCapacity ){
                    minFlow = Graph::gNet[pre[i] ][i]->mCapacity;
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
                    //Graph::gNet[i ][pre[i]]->mCapacity=Graph::gNet[i ][pre[i]]->mCapacity + minFlow;
                    Graph::gNet[i ][pre[i]]->mCapacity=Graph::gNet[i ][pre[i]]->capacity - Graph::gNet[pre[i] ][i]->rCapacity;

                }
                printf("-1 %d -> %d ,minFlow= %.f, capacity= %.f\n",pre[i],i,minFlow,Graph::gNet[pre[i] ][i]->rCapacity);
            }
            else{
                Graph::gNet[pre[i] ][i]->mCapacity = Graph::gNet[pre[i] ][i]->mCapacity -minFlow;
                minCost=minCost + minFlow*Graph::gNet[pre[i] ][i]->cost;
                if(i!=t && pre[i]!=s){
                    Graph::gNet[i ][pre[i]]->rCapacity=Graph::gNet[i ][pre[i]]->capacity - Graph::gNet[pre[i] ][i]->mCapacity;

                    //Graph::gNet[i ][pre[i]]->rCapacity=minFlow;
                }
                printf("+1 %d -> %d ,minFlow= %.f, capacity= %.f\n",pre[i],i,minFlow,Graph::gNet[pre[i] ][i]->mCapacity);
            }
        }
    }
}
bool WeMCMF::spfa() {
    memset(mCost,0x70, sizeof(mCost));
    memset(pre,-1,sizeof(pre));
    mCost[s]=0;
    pre[s]=s;
    isInQue.set(s);
    que.push_back(Graph::netNode[s]->arc);
    Arc *p,*q;
    int dir=1;
    while(!que.empty()){
        p=que.front();
        que.pop_front();
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
