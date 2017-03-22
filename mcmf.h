#ifndef MCMF_H_INCLUDED
#define MCMF_H_INCLUDED

#include "global.h"
struct MCMF
{
    private:

        vector<Link> edge;
        vector<int> head;
        vector<PRE> pre;
        vector<int> dist;
        vector<Path> path;
        int po;
        Path temp;
        int sum_flow;
        int minicost;
        void Init()
        {
            int po=0;
            int sum_flow=0;
            int minicost=0;
            int path_num=0;
            head.resize(link_num,-1);
            edge.resize(link_num*2,Link());

            for(int i:serverID)
                add_edge(source_p,i,INF,0);//构造超源点和部署服务器节点的链路

            for(int i=0;i<node_num;i++)
            {
                for(int j=0;j<node_num;j++)
                {
                    if(gNet[i][j]!=nullptr)
                    {
                        if(gNet[i][j].iswithconsumer==false)
                        add_edge(i,j,*gNet[i][j].capacity,*gNet[i][j].unitcost);
                    }
                }
            }//构造网络链路

            for(int i=0;i<node_num;i++)
            {
                if(netNode[1][i]!=0)
                add_edge(i,sink_p,netNode[1][i],0)
            }//构造连接消费节点的网络节点与超汇点之间的链路；
        }
        //Init负责构造网络图，加入节点，是与外部的接口；
        void add_edge(int u,int v,int cap,int cost)
        {
            edge[po].u=u;
            edge[po].v=v;
            edge[po].cap=cap;
            edge[po].cost=cost;
            edge[po].next=head[u];
            head[u]=po++;

            edge[po].u=v;
            edge[po].v=u;
            edge[po].cap=0;
            edge[po].cost=-cost;
            head[po].next=head[v];
            head[v]=po++;
        }
        bool spfa(int s, int t)
        {
            bitset<MAX_NODE_NUM> mark;
            deque<int> q;
            int cur,cur_v;
            dist.resize(node_num, INF);
            pre.resize(node_num,PRE());

            q.push_front(s);
            mark.set(s);
            dist[s]=0;
            while(!q.empty())
            {
                cur=q.front();
                mark.reset(cur);
                q.pop_front();
                for(int i=head[cur];i!=-1;i=edge[i].next)
                {
                    cur_v=edge[i].v;
                    if(edge[i].cap>0&&dist[cur_v]>dist[cur]+edge[i].cost)
                    {
                        dist[cur_v]=dist[cur]+edge[i].cost;
                        pre[cur_v].n=cur;
                        pre[cur_v].e=i;
                        if(!mark.test(cur_v))
                        {
                            if(q.empty())
                                q.push_front(cur_v);
                            else
                            {
                                if(dist[cur_v]<=dist[q.front()])
                                    q.push_front(cur_v);
                                else
                                    q.push_back(cur_v);
                            }
                            mark.set(cur_v);
                        }
                    }
                }
            }
            return dist[t]!=INF;
        }
        int aug(int s,int t)
        {
            int max_flow=INF;
            for(int i=t;i!=-1;i=pre[i].n)
            {
                max_flow=min(edge[pre[i].e].cap,max_flow);
                temp.nodes.push(i);
            }
            temp.flow=max_flow;
            for(int i=t;i!=-1;i=pre[i].n)
            {
                edge[pre[i].e].cap-=max_flow;
                edge[(pre[i].e)^1].cap+=max_flow;
            }
            sum_flow+=max_flow;
            temp.cost=max_flow*dist[t];
            ++path_num;
            path.push_back(temp);
            return max_flow*dist[t];
        }
    public:
        int path_num;//生成流的路径数量
        void run()
        {
            Init();
            while(spfa(source_p,sink_p))
            {
                minicost+=aug(source_p,sink_p);
                sum_flow+=max_flow;
            }
            if(sum_flow<all_demand) return 0；
            else{
                return minicost;
            }
        }

};
#endif // MCMF_H_INCLUDED
