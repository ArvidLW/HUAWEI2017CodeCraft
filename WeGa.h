/*
 * Copyright 2017
 *
 * ga.h
 *
 * 遗传算法优化初始解
 *
 * 算法思路：
 * 每组服务器为一个个体
 * 第一阶段：增大个体差异
 * 第二阶段：选择优秀基因传承，好个体关联分析
 * 第三阶段：个体变异进化
 *
 * 初始服务器：ServerID,ServerCandidate,ServerPossible
 *
 * 少一个有服务器情景，少两个服务器情景，结果最好的前n个，抽取共同基因片段，做传承基因
 * 如果，少了任何一个服务器价格高于LP价，则尝试增加服务器个数，ln(allServer)
 *
 * Date: March 26, 2017
 * Author: Wei Liu, Ling Bao, Shangyue Zhou
 *
 * $Id: ga.h,v 0.1 2017/03/26 00:30:30 bl Exp $
 *
 *
 * 思路：
 * LP
 * |-serverID,serverCandidate,serverPossible
 * |-minCost,minCostCan,minCostPos
 * |-WeGa
 *     |-preDeal
 *          |-serverID's cost is INF
 *                  |-calcDirect:0
 *                  |-
 *          |-serverID's cost is not INF
 *                  |-reduceOneServer
 *                          |-minCost
 *                          |-gainCommonGene
 *                                  |-minCost
 *                  |-addOneServer
 *                          |-minCost
 *                          |-gainCommonGene
 *                                  |-minCost
 *                  |-compare the minCost to decide the calcDirect(add:1,reduce:-1)
 *     |-evolution
 *          |-set a evolutionCount(we can Change)
 *          |-set a addServerCount(we can Change)
 *          |-set a serverCountRange(we can Change)
 *          |-set the number of Individual to get the CommonGene(we can Change)
 *          |-calcDirect=0
 *          |-calcDirect=-1
 *                  |-random to reduce the server in ServerID
 *                  |-minCost
 *                  |-gainCommonGene
 *                          |-minCost
 *          |-calcDirect=1
 *                  |-random to add the server from ServerCandidate
 *                  |-minCost
 *                  |-gainCommonGene
 *                          |-minCost
 *     |-analyCommonGene(compare all of minCostCommon with minCost)
 *          |- < reduce Server from CommonGene
 *          |- > add Server from CommonGene
 *
 */

#ifndef CDN_WEGA_H
#define CDN_WEGA_H


#define EVOLUTIONCOUNT 10  //进化次数
struct Individual{
    double cost;
    std::bitset<MAXCONSUMER> gene;
    Individual(std::bitset<MAXCONSUMER> b):cost{0},gene{b} {}
    Individual():cost{0} {}

    bool operator < (const Individual &d) const{
        return cost<d.cost;
    }
};

class WeGa {
private:
    char * thefilename;
    std::vector<int> allServer;
    std::bitset<MAXCONSUMER> BESTServer;//保存遗传个体最好基因，标记所选择的服务器
    std::bitset<MAXCONSUMER> LPBestServer;//保存线性规划最好的基因，即选择服务器的最好结果
    std::bitset<MAXCONSUMER> BestGenePart;//最好的基因片段，即选出来的最好的需要传承的基因
    std::vector<Individual> combinIndiv;//种群中优秀的基因组合的个体。包含他们共同的基因
    std::vector<Individual> excellentIndiv;//优秀个体，即能更新minCost的个体
    //std::vector<std::bitset<MAXCONSUMER>> gaPopulation;//种群，从种群中选择好的基因
    std::vector<Individual> gaPopulation;
//    int initPopulationSize;
//    int populationSize;//种群大小
//    int maxIterate;
    int serverCountRange;//在ServerID的计算有效情况下，计算服务器要在ServerID上变动的个数，为ln(allserver)
    double minCost;
    int calcDirect;//计算方向，由预处理决定
    int evolutionCount;//自我进化次数
    int addServerCount;//当CalcDirect方向为1，时增加服务器的个数
    int outStandingCount;//为种群里杰出的几个
public:
    std::vector<int> GServerID;//存放ga选择的服务器节点,经解码过后
private:
    void preDeal();//预处理，判断进行路线
    void reduceOneServer();//在LPserverID中减少一个服务器
    void addOneServer();//增加一个服务器

//    void stageTwoGeneP();//生成种群第二阶段，增加服务器数
//    void chooseBestGene();//精英群体
//    void generatePopulation();//生成种群
//    void decode();//基因解码，即对应真实服务器ID
    void initial();//初始化种群服务器
    double calcCost();//计算适应度
    void calcFitness();//计算适应度
//    void mate();//个体交配，基因片段交换
//    void swap();//交换父子群体
    void evolution();//演变自己进化
    void gainCommonGene();
public:
    WeGa(char * filename):minCost{INF},
                          calcDirect{0},
                          serverCountRange{log(Graph::consumerCount)},
                          evolutionCount{EVOLUTIONCOUNT},
                          addServerCount{1+ChooseServer::serverCandidate.size()/5},
                          outStandingCount{5} {
        allServer.insert(allServer.end(),ChooseServer::serverID.begin(),ChooseServer::serverID.end());
        allServer.insert(allServer.end(),ChooseServer::serverCandidate.begin(),ChooseServer::serverCandidate.end());
        allServer.insert(allServer.end(),ChooseServer::serverPossible.begin(),ChooseServer::serverPossible.end());
        serverCountRange=(int)log(allServer.size());
        thefilename=filename;

    }
    void chooseServer();//ga选择服务器

};

void WeGa::gainCommonGene() {
    printf(splitLine);
    Individual d;
    std::vector<int> serverTmp;
    sort(gaPopulation.begin(),gaPopulation.end());
    printf("check: gapu[0].cost= %.f ,gapu[1].cost= %.f\n",gaPopulation[0].cost,gaPopulation[1].cost);
    for(int i=0;i<ChooseServer::serverID.size()+ChooseServer::serverCandidate.size();++i){
        d.gene[i]=gaPopulation[0].gene[i]&gaPopulation[1].gene[i]&gaPopulation[2].gene[i]&gaPopulation[3].gene[i]&gaPopulation[4].gene[i]&gaPopulation[5].gene[i];
        if(d.gene[i]){
            serverTmp.push_back(allServer[i]);
        }
    }
    d.cost=mc.run(Graph::nodeCount,Graph::arcCount,serverTmp);
    printf("common gene cost: %.f \n",d.cost);
    if(d.cost <minCost){
        printf("we can choose gene from combinIndiv\n");
        minCost=d.cost;
    } else{
        printf("we can go on choose best gene\n");
    }
    BESTServer=d.gene;
    combinIndiv.push_back(d);
}
void WeGa::evolution() {
    printf(splitLine);
    printf("enter evolution!\n");
    printf("calcDirect : %d\n",calcDirect);
    double cost;
    if(calcDirect==-1){
        srand(unsigned(time(NULL)));
        for(int k=0;k<evolutionCount;++k){
            Individual d{LPBestServer};
            //std::bitset<MAXCONSUMER> b{LPBestServer};
            for(int i=0;i<(rand()%serverCountRange)+1;++i){
                d.gene.reset(rand()%ChooseServer::serverID.size());
            }
            //std::cout<<b<<std::endl;
            gaPopulation.push_back(d);
        }
        cost=calcCost();
    }
    if(calcDirect==1){
        srand(unsigned(time(NULL)));
        for(int k=0;k<evolutionCount;++k){
            Individual d{LPBestServer};
            //std::bitset<MAXCONSUMER> b{LPBestServer};
            for(int i=ChooseServer::serverID.size();i<ChooseServer::serverID.size()+rand()%serverCountRange+addServerCount;++i){
                d.gene.set(rand()%ChooseServer::serverCandidate.size()+ChooseServer::serverID.size());
            }
            std::cout<<d.gene<<std::endl;
            gaPopulation.push_back(d);
        }
        cost=calcCost();
    }
    printf("evolution minCost: %.f\n",cost);

}
void WeGa::chooseServer() {
    preDeal();
    evolution();

    for (int j = 0; j < ChooseServer::serverID.size()+ChooseServer::serverCandidate.size(); ++j) {
        if(BESTServer[j]){
            GServerID.push_back(allServer[j]);
        }
    }
    if(minCost<ChooseServer::minCost){
        mc.run(Graph::nodeCount,Graph::arcCount,GServerID);
    }
    else{
        mc.run(Graph::nodeCount,Graph::arcCount,ChooseServer::serverID);
    }

    //std::cout<<"mincost: "<<minCost<<std::endl;
    //write_result(mc.s,thefilename);
}
void WeGa::preDeal() {
    double reduceOneCost,addOneCost;
    initial();
    if(calcDirect==0){
        reduceOneServer();
        reduceOneCost=calcCost();
        printf(splitLine);
        if(reduceOneCost<ChooseServer::minCost){
            calcDirect=-1;
            std::cout<<"we can reduce the server\n";
        }
        addOneServer();
        addOneCost=calcCost();
        if(addOneCost<reduceOneCost){
            calcDirect=1;
            std::cout<<"we can add the server\n";
        }
        printf("lpCost: %.f \nreduceOneCost: %.f \naddOneCost: %.f\n",ChooseServer::minCost,reduceOneCost,addOneCost);

    }
}
void WeGa::addOneServer() {
    for (int j = ChooseServer::serverID.size(); j < ChooseServer::serverID.size()+ChooseServer::serverCandidate.size(); ++j) {
        std::bitset<MAXCONSUMER> b{LPBestServer};
        //printf("j: %d",j);
        b.set(j);
        //std::cout <<b<<std::endl;
        gaPopulation.push_back(b);
    }
    //printf(splitLine);
    //std::cout<<"addOneServer Popu Size : "<<gaPopulation.size()<<std::endl;
}
void WeGa::reduceOneServer() {
    //close a server from ServerID
    for(int i=0;i<ChooseServer::serverID.size();++i){
        std::bitset<MAXCONSUMER> b{LPBestServer};
        b.reset(i);
        gaPopulation.push_back(b);
        //std::cout<<b<<std::endl;
    }
    //printf(splitLine);
    //std::cout<<"reduceOneServer Popu Size : "<<gaPopulation.size()<<std::endl;
    //open a server from ServerCandidate
}
void WeGa::initial() {
    for (int i = 0; i <ChooseServer::serverID.size() ; ++i) {
        LPBestServer.set(i);
    }
    if(ChooseServer::minCost >=INF){
        calcDirect=1;
        printf(splitLine);
        printf("Cannot meet Requirement ServerID\n");
    }
}

double WeGa::calcCost() {

    for (int i = 0; i <gaPopulation.size()  ; ++i) {
        std::vector<int> serverLoc;
        for (int j = 0; j < ChooseServer::serverID.size()+ChooseServer::serverCandidate.size(); ++j) {
            if(gaPopulation[i].gene[j]){
                serverLoc.push_back(allServer[j]);
            }
        }

        gaPopulation[i].cost=mc.run(Graph::nodeCount,Graph::arcCount,serverLoc);

        if(minCost>mc.minicost){
            minCost=mc.minicost;
            BESTServer=gaPopulation[i].gene;
        }
        //serverLoc.clear();
    }
    gainCommonGene();
    gaPopulation.clear();
    return minCost;
//    for (int j = 0; j < ChooseServer::serverID.size()+ChooseServer::serverCandidate.size(); ++j) {
//        if(BESTServer[j]){
//            GServerID.push_back(allServer[j]);
//        }
//    }
//    std::cout<<"mincost: "<<minCost<<std::endl;
//    write_result(m.s,thefilename);
}

//void WeGa::stageTwoGeneP() {
//    for(int i=0;i<ChooseServer::serverID.size();++i){
//        std::bitset<MAXCONSUMER> b;
//        b.bitset();
//        b.reset(i);
//        for (int j= i+1; j < ChooseServer::serverID.size(); ++j) {
//            b.reset(j);
//        }
//        gaPopulation.push_back(b);
//    }
//}




#endif //CDN_WEGA_H
