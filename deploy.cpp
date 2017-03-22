#include "deploy.h"
#include <stdio.h>
#include "mylp.h"
#include "initialData.h"
#include "lpkit.h"
#include <vector>
//#define MODE1 //设定了超源节点变量，缺点，变量多了，不好计算
//#define MODE2 //取消了超源节点变量，并合并了f_{i,j}的双向流量，变为单向，缺点，不好确定服务器位置
#define MODE3 //取消了超源节点变量，合并到网络节点流量差约束中，info[0]+2info[1]个变量，约束为info[0]+2*info[1]个
//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    //printInput(topo,line_num);
    //memset(netNode, -1, sizeof(netNode[0]));//用于初始化标记网络节点所连接的消费节点
    //获取文件第一行信息输出，方便查看对应文件正确性
    getInfo(topo);
    //初始化gNet与cNet
    gNetBuild(topo);
    cNetBuild(topo);
    //为边设置编号，方便在线性规划中设定变量。
    setArcId(gNet,info[0],info[0]);
    //printArc(gNet,info[0],info[0]);
    //printArc(cNet,info[2],info[0]);

    //testLinearP();

    solveLp();
    if(serverID.size()<=0){
        printf("not find server location!\n");
        exit(-1);
    }else{
        printf("Server count: %d\n",serverID.size());
    }

    printVector(serverID);





    std::cout << "Hello, World!" << std::endl;

	// 需要输出的内容
	//char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
    //write_result(topo_file, filename);

}

/**
 * mylpsolve
 * 整数规划模型决策变量个数为，顶点数+两倍的链路数。
 * 约束条件：
 * 服务器变量x为：0-1整数。（个数：顶点数）
 * 带宽消耗为：整数。
 * 每个节点需求流量和b。（个数：顶点数+消费节点数）
 * * 此约束条件在消费节点为给出的，在其它节点大于等于零。
 * * 在消费结点为负，在网络结点为零，在服务器节点为正。
 * 边容量约束。（个数：边数）
 * 服务器位置
 * * 设置一个超源节点流向各个节点，带宽和单价都为0
 * * 超源节点的入边带宽为0，出边带宽为无穷
 * * 超源节点流向哪个节点，则会产生一个服务器费用。故可以约束到目标值。
 * * 所以超源节点有一个关系和x.
 *
 *
 * 变量数，顶点数+边数*2+消费节点到节点的边(消费节点个数)+超源节点到所有节点的边(即顶点数)
 * info[0]+2*info[1]+info[2]+info[0]=2*info[0]+2*info[1]+info[2]
 * 其中消费节点到节点的边(消费节点个数)的需求必须满足，所以是给定的
 * 所以变量数为：info[0]+2*info[1]+info[0]=2*info[0]+2*info[1]
 * 目标函数变量数 顶点数+边数*2,注意边数顺序为数组从左至右从上至下遍历的边，然后再重复一次相反的边
 * info[0]+2*info[1]
 * v0,v1,...,vn
 *
 * 约束个数
 * 网络中间节点流量为0：info[0]个，即节点个数
 * 网络服务器位置与超源节点关系约束（两边）：info[0]*2个，即节点个数
 * 超源节点总共流量约束：1个
 * 一共3*info[0]+1个约束
 *
 * 实际求时由于要加个松弛变量2*info[0]，和人工变量info[0]+1（由于等式）,为前info[0]个与最后一个
 * 还要加一个等式右边约束值b
 * 这样总的变量为5*info[0]+1+2*info[1]+1=5*info[0]+2+2*info[1]
 * @return  [description]
 */
bool solveLp(){
    /**lpsolve建立开始**********/
    lprec *lp;
#ifdef MODE3
    lp = make_lp(0,info[0]+2*info[1]);

    /**lpsolve建立结束**********/
    /**目标函数开始************/
    double myoj[1+info[0]+2*info[1]];
    memset(myoj,0,sizeof(myoj));
    int i,j,k;
    double vx[1+info[0]], ex[info[1]];
    //vx
    for(i=1; i<=info[0]; ++i){
        vx[i]=info[3];
    }

    //ex
    for(i=0,k=0; i<info[0]; ++i){
        for(j=i; j<info[0]; ++j){
            if(gNet[i][j]!=NULL){
                ex[k]=gNet[i][j]->unitcost;
                ++k;
            }

        }
    }
    //拼接数组
    printf("s0:%0.0f\n",myoj[1]);
    joinArr(myoj,1+info[0]+2*info[1],vx,1+info[0],ex,info[1]);
    printf("s1:%0.0f\n",myoj[1]);
    printArr(myoj,1+info[0]+2*info[1]);
    printf("s2\n");
    //加入到lp中
    set_obj_fn(lp, myoj);
    //print_lp(lp);
    printf("object function set successful!\n");
    /**目标函数结束**********/

    /**约束条件开始**********/
    /**网络节点流量差为0约束开始***/
    double b[1+info[0]+2*info[1]];
    /**约束*/
    //若只加与消费节点相连的网络节点约束，但这样中间节点流量差不为0目标值会比实际小
    for(i=0;i<info[0];++i){
        //i=consumerNode[k];
        memset(b,0,sizeof(b));
        //注意这里没用j=i因为是要对每个节点的流入流出分析，而提供约束，由于上三角和下三角指的结点是一样的，所以都有n
        for(j=0;j<info[0];++j){
            if(gNet[i][j]!=NULL){
                //访问时索引不用加1
                if(i<j){
                    b[info[0]+gNet[i][j]->n ]=1;//正向
                    b[info[0]+info[1]+gNet[i][j]->n ]=-1;//反向
                }
                else{
                    b[info[0]+info[1]+gNet[i][j]->n ]=1;//正向
                    b[info[0]+gNet[i][j]->n ]=-1;//反向
                }

            }
        }
        /**服务器约束,中间节点服务器流入流量=节点流出流量*/
        b[i+1]=-2000;
        //连接消费节点,由于节点索引从0开始，所以+1
        //printArr(b,1+2*info[0]+2*info[1]);
        //EQ变为LE
        add_constraint(lp, b, LE, (-1)*netNode[1][i]);

    }
    printf("network node flow is 0 constraint success!\n");
    /**网络节点流量差为0约束结束***/
    /**网络链路流量取值约束开始***/
    for(i=0;i<info[0];++i){
        for(j=i;j<info[0];++j){
            if(gNet[i][j]!=NULL){
                set_upbo(lp, info[0]+gNet[i][j]->n, gNet[i][j]->capacity);
                set_upbo(lp, info[0]+info[1]+gNet[i][j]->n, gNet[i][j]->capacity);
            }
        }
    }
    printf("network road capacity value constraint success!\n");
    /**网络链路流量取值约束结束***/

    /**约束条件结束**********/
#endif
#ifdef MODE2
    lp=make_lp(0,info[0]+info[1]);
    double myoj[1+info[0]+info[1]];
    memset(myoj,0,sizeof(myoj));
    int i,j,k;
    //目标
    double vx[1+info[0]], ex[info[1]];
    //vx
    for(i=1; i<=info[0]; ++i){
        vx[i]=info[3];
    }

    //ex
    for(i=0,k=0; i<info[0]; ++i){
        for(j=i; j<info[0]; ++j){
            if(gNet[i][j]!=NULL){
                ex[k]=gNet[i][j]->unitcost;
                ++k;
            }

        }
    }
    //拼接
    for(i=0;i<1+info[0];++i){
        myoj[i]=vx[i];
    }
    for(;i<1+info[0]+info[1];++i){
        myoj[i]=ex[i];
    }
    set_obj_fn(lp, myoj);

    /**消费节点需求约束条件*/
    double b[1+info[0]+info[1]];

    for(k=0;k<info[3];++k){
        i=consumerNode[k];
        memset(b,0,sizeof(b));
        //注意这里没用j=i因为是要对每个节点的流入流出分析，而提供约束，由于上三角和下三角指的结点是一样的，所以都有n
        for(j=0;j<info[0];++j){
            if(gNet[i][j]!=NULL){
                //访问时索引不用加1
                b[info[0]+gNet[i][j]->n ]=1;//正向

            }
        }
        //大于消费节点需求
        //printArr(b,1+2*info[0]+2*info[1]);
        add_constraint(lp, b, GE, netNode[1][i]);

    }
    /**网络链路流量取值约束开始***/
    for(i=0;i<info[0];++i){
        for(j=i;j<info[0];++j){
            if(gNet[i][j]!=NULL){
                set_upbo(lp, info[0]+gNet[i][j]->n, gNet[i][j]->capacity);
            }
        }
    }
    /**服务器与节点的关系约束*/
    for(i=0;i<info[0];++i){
        memset(b,0,sizeof(b));
        for(j=0;j<info[0];++j){
            if(gNet[i][j]!=NULL){
                //访问时索引不用加1
                b[info[0]+gNet[i][j]->n ]=1;//正向


            }
        }
        b[i+1]=-2000;
        add_constraint(lp, b, LE, 0);
        //printf("netNode[1][i]=%d\n", (-1)*netNode[1][i]);
    }
#endif
#ifdef MODE1
    lp = make_lp(0,2*info[0]+2*info[1]);

    /**lpsolve建立结束**********/
    /**目标函数开始************/
    double myoj[1+info[0]+2*info[1]];
    memset(myoj,0,sizeof(myoj));
    int i,j,k;
    double vx[1+info[0]], ex[info[1]];
    //vx
    for(i=1; i<=info[0]; ++i){
        vx[i]=info[3];
    }

    //ex
    for(i=0,k=0; i<info[0]; ++i){
        for(j=i; j<info[0]; ++j){
            if(gNet[i][j]!=NULL){
                ex[k]=gNet[i][j]->unitcost;
                ++k;
            }

        }
    }
    //拼接数组
    printf("s0:%0.0f\n",myoj[1]);
    joinArr(myoj,1+info[0]+2*info[1],vx,1+info[0],ex,info[1]);
    printf("s1:%0.0f\n",myoj[1]);
    printArr(myoj,1+info[0]+2*info[1]);
    printf("s2\n");
    //加入到lp中
    set_obj_fn(lp, myoj);
    //print_lp(lp);
    printf("object function set successful!\n");
    /**目标函数结束**********/

    /**约束条件开始**********/
    /**网络节点流量差为0约束开始***/
    double b[1+2*info[0]+2*info[1]];

    //结点i连接网络节点,注意超源节点会流向它
    for(i=0;i<info[0];++i){
        /*if(netNode[0][i]==-1){
            continue;
        }*/
        memset(b,0,sizeof(b));
        //注意这里没用j=i因为是要对每个节点的流入流出分析，而提供约束，由于上三角和下三角指的结点是一样的，所以都有n
        for(j=0;j<info[0];++j){
            if(gNet[i][j]!=NULL){
                //访问时索引不用加1
                if(i<j){
                    b[info[0]+gNet[i][j]->n ]=1;//正向
                    b[info[0]+info[1]+gNet[i][j]->n ]=-1;//反向
                }
                else{
                    b[info[0]+info[1]+gNet[i][j]->n ]=1;//正向
                    b[info[0]+gNet[i][j]->n ]=-1;//反向
                }

            }
        }
        //超源节点流入
        b[info[0]+2*info[1]+i+1]=-1;
        //连接消费节点,由于节点索引从0开始，所以+1
        //printArr(b,1+2*info[0]+2*info[1]);
        //EQ变为LE
        add_constraint(lp, b, LE, (-1)*netNode[1][i]);
        //printf("netNode[1][i]=%d\n", (-1)*netNode[1][i]);
    }
    //与上相比只加与消费节点相连的网络节点约束，但是有可能网络节点的流量差不为零，使原有目标比实际小
//    for(k=0;k<info[3];++k){
//        i=consumerNode[k];
//        memset(b,0,sizeof(b));
//        //注意这里没用j=i因为是要对每个节点的流入流出分析，而提供约束，由于上三角和下三角指的结点是一样的，所以都有n
//        for(j=0;j<info[0];++j){
//            if(gNet[i][j]!=NULL){
//                //访问时索引不用加1
//                if(i<j){
//                    b[info[0]+gNet[i][j]->n ]=1;//正向
//                    b[info[0]+info[1]+gNet[i][j]->n ]=-1;//反向
//                }
//                else{
//                    b[info[0]+info[1]+gNet[i][j]->n ]=1;//正向
//                    b[info[0]+gNet[i][j]->n ]=-1;//反向
//                }
//
//            }
//        }
//        //超源节点流入
//        b[info[0]+2*info[1]+i+1]=-1;
//        //连接消费节点,由于节点索引从0开始，所以+1
//        //printArr(b,1+2*info[0]+2*info[1]);
//        //EQ变为LE
//        add_constraint(lp, b, LE, (-1)*netNode[1][i]);
//
//    }
    printf("network node flow is 0 constraint success!\n");
    /**网络节点流量差为0约束结束***/
    /**网络链路流量取值约束开始***/
    for(i=0;i<info[0];++i){
        for(j=i;j<info[0];++j){
            if(gNet[i][j]!=NULL){
//                set_lowbo(lp, info[0]+gNet[i][j]->n, 0);
                set_upbo(lp, info[0]+gNet[i][j]->n, gNet[i][j]->capacity);

//                set_lowbo(lp, info[0]+info[1]+gNet[i][j]->n, 0);
                set_upbo(lp, info[0]+info[1]+gNet[i][j]->n, gNet[i][j]->capacity);
            }
        }
    }
    printf("network road capacity value constraint success!\n");
    /**网络链路流量取值约束结束***/
    /**网络链路与超源节点流量约束开始***/
    int usj=2000;
    for(i=1;i<info[0]+1;++i){
//        memset(b,0,sizeof(b));
//        b[i]=1;
//        b[i+info[0]+2*info[1]]=-1;
//        add_constraint(lp, b, LE, 0);
        //printf("----\n");
        //printArr(b,1+2*info[0]+2*info[1]);
        memset(b,0,sizeof(b));
        b[i]=usj;
        b[i+info[0]+2*info[1]]=-1;
        add_constraint(lp, b, GE, 0);
        //printf("----\n");
        //printArr(b,1+2*info[0]+2*info[1]);
    }
    printf("network link and super node constraint success!\n");
    /**网络链路与超源节点流量约束结束***/
    /**超源节点流量总值约束开始***/
//    memset(b,0,sizeof(b));
//    for(i=info[0]+2*info[1]+1;i<=2*info[0]+2*info[1];++i){
//        b[i]=1;
//    }
//    int total=0;
//    for(i=0;i<info[0];++i){
//        total=total+netNode[1][i];
//    }
//    //EQ改为GE,表示最小满足
//    add_constraint(lp, b, GE, total);
//    printf("total=%d\n",total);
    /**超源节点流量总值约束结束***/
//    /**约束转换服务器节点标记加起来大于1，至少有一个为服务器，松约束**/
//    memset(b,0,sizeof(b));
//    for(i=1;i<=info[0];++i){
//        b[i]=1;
//    }
//    add_constraint(lp, b, GE, 1);


//    /**整数约束开始，不需要整数约束，因为边带宽为整数***/
//    for(i=51;i<=2*info[0]+2*info[1];++i){
//        set_int(lp, i, TRUE);
//    }
//    printf("整数约束,执行成功\n");
//    /**整数约束结束***/
//    /**{0,1}约束开始***/
//    for(i=1;i<=info[0];++i){
//        set_int(lp, i, TRUE);
//    }
//    printf("{0,1}约束执行成功\n");
//    /**{0,1}约束结束***/

    /**约束条件结束**********/
#endif
    //set_timeout(lp, 60);
    //set_simplextype(lp, SIMPLEX_PRIMAL_DUAL);
    //reset_basis(lp);
    solve(lp);
    //print_objective(lp);
    print_solution(lp);
    printf("run success\n");
    getServeLocation(lp);
    delete_lp(lp);
    return true;

}

void getServeLocation(lprec *lp){
    for(int i=1;i<=info[0];++i){
        //浮点!=0判断，如果大于MIN_VALUE则变量值不为0，记为服务器位置，记录服务器编号
        if((double)lp->best_solution[lp->rows+i] > MIN_VALUE ){
            serverID.push_back(i-1);//网络节点从零开始编号，在线性规划中变量从1开始，而前info[0]个为网络节点
        }
    }
}

void printVector(std::vector<int> v){
    for(int i=0;i<(signed)v.size();++i){
        std::cout<< v[i] << " ";
    }
    std::cout<<std::endl;
}


