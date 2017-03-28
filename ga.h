/*
 * Copyright 2017
 *
 * ga.h
 *
 * 遗传算法优化初始解
 *
 * Date: March 26, 2017
 * Author: Wei Liu, Ling Bao, Shangyue Zhou
 *
 * $Id: ga.h,v 0.1 2017/03/26 00:30:30 bl Exp $
 *
 */

#ifndef CDN_GA_H
#define CDN_GA_H


#define MAX_COST 1000000000
#define MAX_FITNESS 1000000000.0
//#define Zkw
#define Mcmf


/**
 * 存储任意两点之间的距离和路径，Floyd算法
 * */
int VertexCost[MAXNODE][MAXNODE];
int VertexPath[MAXNODE][MAXNODE];
std::vector<int> getPath;


struct GA {
    // Floyd计算任意两点之间的距离
    int ComputeCost(Arc *gMatrix[][MAXNODE], int vexNum);

    // 递归寻路
    void  Prn_Pass(int j , int k);
    // 获取两点之间的路径
    bool GetPath(int i, int j);
};


int GA::ComputeCost(Arc *gMatrix[][MAXNODE], int vexNum) {
    for (int i = 0;i < vexNum;i++) {
        for (int j = 0;j < vexNum;j++) {
            if (i == j) {
                VertexCost[i][j] = 0;
            }
            else if(gMatrix[i][j] != nullptr) {
                VertexCost[i][j] = (gMatrix[i][j]->capacity) * (gMatrix[i][j]->cost);
            }
            else {
                VertexCost[i][j] = MAX_COST;
            }
            VertexPath[i][j] = -1;
        }
    }

    //关键代码部分
    for (int k = 0;k < vexNum;k++) {
        for (int i = 0;i < vexNum;i++) {
            for (int j = 0;j < vexNum;j++) {
                if (VertexCost[i][k] + VertexCost[k][j] < VertexCost[i][j]) {
                    VertexCost[i][j] = VertexCost[i][k] + VertexCost[k][j];
                    VertexPath[i][j] = k;
                }
            }
        }
    }

    return 0;
}

// 获取两点之间的路径
void  GA::Prn_Pass(int j , int k) {
    if (VertexPath[j][k]!=-1) {
        Prn_Pass(j,VertexPath[j][k]);
        std::cout<<"-->"<<VertexPath[j][k];
        getPath.push_back(VertexPath[j][k]);
        Prn_Pass(VertexPath[j][k],k);
    }
}

bool GA::GetPath(int i, int j) {
    std::cout<<i<<"到"<<j<<"的最短路径为:";
    std::cout<<i;
    Prn_Pass(i, j);
    std::cout<<"-->"<<j<<std::endl;
    std::cout<<"最短路径长度为:"<<VertexCost[i][j]<<std::endl;

    if (getPath.empty() == false) {
        return false;
    }

    return true;
}


// 遗传算法实现函数
class OurGA {
private:
    int ga_size = 50; // 种群大小 2048
    int ga_max_iterate = 80;	// 最大迭代次数 16384
    float ga_elitism_rate = 0.10f; // 精英比率 0.10f
    float ga_mutation_rate  = 0.05f; // 变异率 0.25f
    float ga_mutation = RAND_MAX * ga_mutation_rate;
    std::vector<int> ga_server = ChooseServer::serverID; // ChooseServer::serverCandidate;
    std::vector<int> tmp_server = ChooseServer::serverID; // ChooseServer::serverCandidate;
    int ga_target_size = ChooseServer::serverID.size(); // 目标大小，即最优服务器可能包含的数量

    // 遗传算法结构体，包括适应与最优解
    struct ga_struct {
        std::string str; // 个体基因编码串
        double fitness; // 个体适应度
    };
    typedef std::vector<ga_struct> ga_vector;// 合并写法

public:
    // 初始化化参数
    OurGA() {
        std::cout<<"使用默认初始参数！"<<std::endl;
    }

    // 初始化函数
    OurGA(int size, int max_iterate, float elitism_rate, float mutation_rate) {
        ga_size = size;
        ga_max_iterate = max_iterate;
        ga_elitism_rate = elitism_rate;
        ga_mutation_rate = mutation_rate;
    }

    // 初始化种群（服务器）
    void init_population_server(ga_vector &population, ga_vector &buffer ) {
        for (int i=0; i<ga_size; i++) {
            ga_struct citizen;

            citizen.fitness = 0.0;
            citizen.str.erase();

            // 初始化个体基因
            if (i == 0) {
                // 将最优个体基因加入种群
                for (int j=0; j<ga_target_size; j++) {
                    citizen.str += std::to_string(1);
                }
            }
            else {
                // 初始化其余个体基因
                for (int j=0; j<ga_target_size; j++) {
                    citizen.str += std::to_string(rand() % 2);
                }
            }

            // 将个体放入种群
            population.push_back(citizen);
        }

        // 分配一个存储空间
        buffer.resize(ga_size);
    }

    // 计算适应度（服务器）
    void calc_fitness_server(ga_vector &population) {

//        for (int i=0; i<ga_size; i++) {
//            std::cout<<(population)[i].str<<std::endl;
//        }

        for (int i=0; i<ga_size; i++) {
            // 清空服务器节点放置缓存
            std::vector <int>().swap(ChooseServer::serverID);
            for (int j=0; j<ga_target_size; j++) {
                if (population[i].str[j] == '1') {
                    ChooseServer::serverID.push_back(ga_server[j]);
                }
            }

            //ChooseServer::printVector(ChooseServer::serverID);

            if (!ChooseServer::serverID.empty()) {
#ifdef Mcmf
                MCMF mcmf;
                mcmf.run(Graph::nodeCount,Graph::arcCount);
                population[i].fitness = mcmf.minicost;
#endif // Mcmf
#ifdef Zkw
                ZKW zkw;
                zkw.run(Graph::nodeCount,Graph::arcCount);
                population[i].fitness = zkw.minicost;
#endif // Zkw
                //printf("Fitness:%.f\n", population[i].fitness);
            }
            else {
                // 没有服务节点的个体
                population[i].fitness = MAX_FITNESS;
            }
        }
    }

    //  // 根据适应度对个体进行排序，从小到大排序
    static bool fitness_sort(ga_struct x, ga_struct y) {
        return (x.fitness < y.fitness);
    }
    inline void sort_by_fitness(ga_vector &population) {
        std::sort(population.begin(), population.end(), fitness_sort);
    }

    // 精英群体
    void elitism(ga_vector &population, ga_vector &buffer, int esize) {
        for (int i=0; i<esize; i++) {
            buffer[i].str = population[i].str;
            buffer[i].fitness = population[i].fitness;
        }
    }

    // 突变操作（服务器）
    void mutate_server(ga_struct &member) {
        int ipos = rand() % ga_target_size;

        if (rand() % 2 == 1){
            member.str[ipos] = '1';
        }
        else {
            member.str[ipos] = '0';
        }
        //std::cout<<member.str[ipos]<<std::endl;
    }

    // 交换操作（服务器）
    void mate_server(ga_vector &population, ga_vector &buffer) {
        int spos;
        int i1;
        int i2;
        int esize = ga_size * ga_elitism_rate;

        // 精英群体
        elitism(population, buffer, esize);

        // 与其它个体基因进行交换
        for (int i=esize; i<ga_size; i++) {
            i1 = rand() % (ga_size / 2);
            i2 = rand() % (ga_size / 2);
            spos = rand() % ga_target_size;

            buffer[i].str = population[i1].str.substr(0, spos) +
                            population[i2].str.substr(spos, ga_target_size - spos);

            if (rand() < ga_mutation) mutate_server(buffer[i]);
        }
    }

    // 打印输出本次迭代最好的个体
    inline void print_best(ga_vector &gav) {
        std::cout << "Best: " << gav[0].str << " (" << gav[0].fitness << ")" << std::endl;
    }

    // 基因解码
    void decode(ga_vector &gav, char *filename) {
        // 清空服务器节点放置缓存
        std::vector <int>().swap(ChooseServer::serverID);
        //ChooseServer::serverID.clear();

        for (int i = 0; i < gav[0].str.size(); ++i) {
            if (gav[0].str[i] == '1') {
                ChooseServer::serverID.push_back(ga_server[i]);
            }
        }

        if (!ChooseServer::serverID.empty()) {
#ifdef Mcmf
            MCMF mcmf;
            mcmf.run(Graph::nodeCount,Graph::arcCount);
            write_result(mcmf.s,filename);
#endif // Mcmf
#ifdef Zkw
            ZKW zkw;
            zkw.run(Graph::nodeCount,Graph::arcCount);
            write_result(zkw.s,filename);
#endif // Zkw

            //printf("Minicost:%.1f\n", gav[0].fitness);
        }
    }

    // 交换父子群体
    void swap(ga_vector *&population, ga_vector *&buffer) {
        ga_vector *temp = population;
        population = buffer;
        buffer = temp;
    }

    bool GaAlgorithmServer(std::vector<int> InitialServer, std::vector<int> AlternataServer,
                           int vertexNum, int edgeNum, char *filename) {
        // 用于产生伪随机数的时间种子
        srand(unsigned(time(NULL)));

        ga_vector pop_alpha, pop_beta;
        ga_vector *population, *buffer;

        init_population_server(pop_alpha, pop_beta);

        population = &pop_alpha;
        buffer = &pop_beta;

        for (int i=0; i<ga_max_iterate; i++) {
            calc_fitness_server(*population);		// 计算适应度
            sort_by_fitness(*population);	// 对个体进行排序
            print_best(*population);		// 输出最好的个体
            decode(*population, filename);  // 基因解码

            if ((*population)[0].fitness == 0) break;

            mate_server(*population, *buffer);		// 个体基因交换
            swap(population, buffer);		// 交换父子群体
        }

        return 0;
    }
};


#endif //CDN_GA_H