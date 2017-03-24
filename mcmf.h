#ifndef MCMF_H_INCLUDED
#define MCMF_H_INCLUDED
#define Debug
#include "global.h"
#include "initialData.h"
int source_p;//��Դ��
int sink_p;//�����
int node_num;//����������
int link_num;//������
int all_demand;//�������ѽ���������

struct MCMF
{
    private:

        vector<Link> edge;
        vector<int> head;
        vector<PRE> pre;
        vector<int> dist;
        vector<Path> path;
        int po;
        int sum_flow;
        int minicost;
        int path_num;//��������·������

        void Init(int num_n,int num_e)
        {
            po=0;
            sum_flow=0;
            minicost=0;
            path_num=0;
            node_num=num_n+2;
            link_num=num_e;
            int edgesize=link_num*2+serverID.size()+info[2];
            source_p =node_num-2;
            sink_p=node_num-1;
            all_demand=0;
            head.resize(node_num,-1);
            edge.resize(edgesize*2,Link());
            path.clear();

            for(int i : serverID)
            {
                add_edge(source_p,i,INF,0);//���쳬Դ��Ͳ���������ڵ����·
            }
            for(int i=0;i<node_num;i++)
            {
                for(int j=0;j<node_num;j++)
                {
                    if(gNet[i][j]!=nullptr)
                    {
                        add_edge(i,j,(*gNet[i][j]).capacity,(*gNet[i][j]).unitcost);
                    }
                }
            }//����������·
            for(int i=0;i<node_num;i++)
            {
                if(netNode[1][i]!=0)
                {
                    add_edge(i,sink_p,netNode[1][i],0);
                    all_demand+=netNode[1][i];
                }
            }//�����������ѽڵ������ڵ��볬���֮�����·��
        }
        //Init����������ͼ������ڵ㣬�����ⲿ�Ľӿڣ�
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
            edge[po].next=head[v];
            head[v]=po++;
        }
        bool spfa(int s, int t)
        {
            bitset<MAX_NODE_NUM> mark;
            deque<int> q;
            int cur,cur_v;
            dist.clear();
            dist.clear();
            dist.resize(node_num,INF);
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
            return dist[t]<INF;
        }
        int aug(int s,int t)
        {
            int max_flow=INF;
            Path temp;
            for(int i=t;i!=-1;i=pre[i].n)
            {
                if(i!=s)
                {
                    max_flow=min(edge[pre[i].e].cap,max_flow);
                }
                temp.nodes.push_front(i);
            }
            temp.flow=max_flow;
            for(int i=t;i!=-1;i=pre[i].n)
            {
                edge[pre[i].e].cap-=max_flow;
                edge[(pre[i].e)^1].cap+=max_flow;
            }
            temp.cost=max_flow*dist[t];
            ++path_num;
            path.push_back(temp);
            return max_flow;
        }
    public:
        char s[80000];
        void run(int num1,int num2)
        {
            Init(num1,num2);
            while(spfa(source_p,sink_p))
            {
                int tmpflow=aug(source_p,sink_p);
                minicost+=tmpflow*dist[sink_p];
                sum_flow+=tmpflow;
            }

            if(sum_flow<all_demand)
            {
                cout<<"NA\n"<<endl;
            }
            else{
                sprintf(s,"%d\n",path_num);
                for(Path pa : path)
                {
                    pa.nodes.pop_front();
                    pa.nodes.pop_back();
                    int t=pa.nodes.back();
                    pa.nodes.push_back(netNode[0][t]);
                    char s1[1000];
                    for(int n : pa.nodes)
                    {
                            sprintf(s1,"%d ",n);
                            strcat(s,s1);
                    }
                    sprintf(s1,"%d\n",pa.flow);
                    strcat(s,s1);
                }
                cout<<s<<endl;
                minicost+=serverID.size()*info[3];
            }
        }

};
#endif // MCMF_H_INCLUDED
