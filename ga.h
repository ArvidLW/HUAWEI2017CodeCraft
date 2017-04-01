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


#define MAX_COST 1e8
// 85000000
#define TIME_END 85000000


// 遗传算法实现函数
class OurGA {
private:
    // 默认初始化参数
    int ga_size = 50; // 种群大小 2048
    int ga_max_iterate = 32000;	// 最大迭代次数 16384
    float ga_elitism_rate = 0.25f; // 精英比率 0.10f
    int esize;
    float ga_mutation_rate  = 0.25f; // 变异率 0.25f
    float ga_mutation;
    int decay_step = 50;
    float decay_rate = 0.05;
    int ga_step;
    int mutate_step = 50;
    int high = 10;
    int middle = 90;

    //ZKW ga_run;
    MCMF ga_run;

    // 网络相关的动态初始化参数
    std::vector<int> ga_server = ChooseServer::serverID; // 工作原码
    int ga_target_size = ChooseServer::serverID.size(); // 工作基因长度
    double ga_minicost = INF;
    std::string ga_s;
    char *ga_filename;

    // 与个体基因编码解码相关的参数
    std::vector<int> ga_Id_server = ChooseServer::serverID;
    int size_Id_server = ChooseServer::serverID.size();
    std::vector<int> ga_Candidate_server = ChooseServer::serverCandidate;
    int size_Candidate_server = ChooseServer::serverCandidate.size();
    std::vector<int> ga_possible_server = ChooseServer::serverPossible;
    int size_Possible_server = ChooseServer::serverPossible.size();

    // 遗传算法结构体，包括适应与最优解
    struct ga_struct {
        std::string str; // 个体基因编码串
        double fitness; // 个体适应度
    };
    typedef std::vector<ga_struct> ga_vector;// 合并写法

public:
    bool bSolve;
    // 初始化化参数
    OurGA(char *filename) {
        std::cout<<"使用GA默认初始参数！"<<std::endl;
        ga_filename = filename;

        ChooseServer::serverID.insert(ChooseServer::serverID.end(),
                                      ga_Candidate_server.begin(), ga_Candidate_server.end());
        ChooseServer::serverID.insert(ChooseServer::serverID.end(),
                                      ga_possible_server.begin(), ga_possible_server.end());

        // 初始化工作基因变量
        ga_target_size = ChooseServer::serverID.size();
        ga_server = ChooseServer::serverID;
        esize = ceil(ga_size * ga_elitism_rate);
        ga_mutation = RAND_MAX * ga_mutation_rate;

        std::cout<<"GA length:"<<ga_target_size<<std::endl;

        //ZKW ga_run;
        ga_run.run(Graph::nodeCount,Graph::arcCount, ChooseServer::serverID);
        if (!(ga_run.minicost < INF)) {
            printf("No LP Solve!\n");

            bSolve = false;
        }
        else {
            write_result(ga_run.getRoute(), ga_filename);
            bSolve = true;
        }
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
            citizen.fitness = INF;
            citizen.str.erase();

            // 初始化个体基因
            if (i == 0) {
                // 将最优个体基因加入种群
                for (int j=0; j<ga_target_size; j++) {
                    citizen.str += std::to_string(1);
                }
                ga_s.clear();
                ga_s = citizen.str;
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

        //PrintGA(population);
    }

    // 计算适应度（服务器）
    void calc_fitness_server(ga_vector &population, int step) {
        // 查看种群个体基因序列
        //PrintGA(population);
        int tmp_esize;
        if (step == 0) {
            tmp_esize = 0;
        }
        else {
            tmp_esize = esize;
        }

        for (int i=esize; i<ga_size; i++) {
            // 清空服务器节点放置缓存
            std::vector <int>().swap(ChooseServer::serverID);
            for (int j=0; j<ga_target_size; j++) {
                if (population[i].str[j] == '1') {
                    ChooseServer::serverID.push_back(ga_server[j]);
                }
            }

            // 查看解码后服务器所在位置
            //ChooseServer::printVector(ChooseServer::serverID);

            // 计算个体适应度
            if (!ChooseServer::serverID.empty()) {
                //ZKW ga_run;
                ga_run.run(Graph::nodeCount,Graph::arcCount,ChooseServer::serverID);
                population[i].fitness = ga_run.minicost;
            }
            else {
                // 没有服务节点的个体
                population[i].fitness = INF;
            }
        }
    }

    // 根据适应度对个体进行排序，从小到大排序
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
        int choose = mutate_design();
        int ipos;
        if (choose == 1) {
            if (size_Id_server != 0) {
                ipos = rand() % size_Id_server;
            }
            else {
                ipos = 0;

                printf("May be something wrong!\n");
            }
        }
        else if (choose == 2) {
            if (size_Candidate_server != 0) {
                ipos = size_Id_server + rand() % size_Candidate_server;
            }
            else {
                ipos = rand() % size_Id_server;
            }
        }
        else {
            if (size_Possible_server != 0) {
                ipos = size_Id_server + size_Candidate_server + rand() % size_Possible_server;
            }
            else {
                ipos = rand() % size_Id_server;
            }
        }

        (member.str[ipos] == '1') ? (member.str[ipos] = '0') : (member.str[ipos] = '1');
    }

    // 突变概率设计
    int mutate_design() {
        int g = rand() % 100;

        if (ga_step % (mutate_step - 1) == 0) {
            if (high < 45) {
                high += 1;
            }
            if (middle > 55) {
                middle -= 2;
            }
        }

        if (g < high) {
            return 1;
        }
        else if (g >= high && g < middle) {
            return 2;
        }
        else {
            return 3;
        }
    }

    // 交换操作（服务器）
    void mate_server(ga_vector &population, ga_vector &buffer) {
        int spos;
        int i1;
        int i2;

        // 精英群体（遗传）
        elitism(population, buffer, esize);

        // 与其它个体基因进行变异
        for (int i=esize; i<ga_size; i++) {
            i1 = rand() % ga_size;
            i2 = rand() % ga_size;
            spos = rand() % ga_target_size;

            // 基因交换
            buffer[i].str = population[i1].str.substr(0, spos) +
                            population[i2].str.substr(spos, ga_target_size - spos);

            // 基因位变异
            if (rand() < ga_mutation) mutate_server(buffer[i]);
        }
    }

    // 打印输出本次迭代最好的个体
    inline void print_best(ga_vector &gav) {
        //std::cout << "Best: " << gav[0].str << " (" << gav[0].fitness << ")" << std::endl;
        ga_s.clear();
        ga_s = gav[0].str;
        ga_minicost = gav[0].fitness;
    }

    // 基因解码
    void decode() {
        // 清空服务器节点放置缓存
        //std::vector <int>().swap(ChooseServer::serverID);
        ChooseServer::serverID.clear();

        for (int i = 0; i < ga_s.size(); ++i) {
            if (ga_s[i] == '1') {
                ChooseServer::serverID.push_back(ga_server[i]);
            }
        }

        if (!ChooseServer::serverID.empty()) {
            //ZKW ga_run;
            ga_run.run(Graph::nodeCount,Graph::arcCount, ChooseServer::serverID);
            if (ga_run.minicost < INF) {
                write_result(ga_run.getRoute(),ga_filename);
            }
        }
    }

    // 交换父子群体
    void swap(ga_vector *&population, ga_vector *&buffer) {
        ga_vector *temp = population;
        population = buffer;
        buffer = temp;
    }

    // 打印基因序列
    void PrintGA(ga_vector &population) {
        for (int i=0; i<ga_size; i++) {
            std::cout<<(population)[i].str<<std::endl;
        }
    }

    // 精英率与变异率衰减
    void decay(int step) {
        int decay_e_rate;
        if (ga_target_size < 100) {
            decay_e_rate = 0.70;
        }
        else if ((ga_target_size >= 100) && (ga_target_size < 200)) {
            decay_e_rate = 0.75;
        }
        else if ((ga_target_size >= 200) && (ga_target_size < 300)) {
            decay_e_rate = 0.80;
        }
        else if ((ga_target_size >= 300) && (ga_target_size < 400)) {
            decay_e_rate = 0.85;
        }
        else {
            decay_e_rate = 0.90;
        }


        if ((step % decay_step) == (decay_step - 1)) {
            if (ga_elitism_rate * (1 + decay_rate) < decay_e_rate) {
                ga_elitism_rate *= (1 + decay_rate);
            }
            esize = ceil(ga_size * ga_elitism_rate);

            if (ga_mutation_rate * (1 + decay_rate) < 0.99) {
                ga_mutation_rate *= (1 + decay_rate);
            }
            ga_mutation = RAND_MAX * ga_mutation_rate;
        }
    }

    bool GaAlgorithmServer() {
        // 用于产生伪随机数的时间种子
        srand(unsigned(time(NULL)));

        ga_vector pop_alpha, pop_beta;
        ga_vector *population, *buffer;

        init_population_server(pop_alpha, pop_beta);

        population = &pop_alpha;
        buffer = &pop_beta;

        int t0=clock();
        for (int i=0; i<ga_max_iterate; i++) {
            ga_step = i;

            decay(i);
            calc_fitness_server(*population, i);		// 计算适应度
            sort_by_fitness(*population);	// 对个体进行排序
            print_best(*population);		// 输出最好的个体

            if (((clock() - t0) > TIME_END) || (i == ga_max_iterate-1)) {
                decode();  // 基因解码
                std::cout<<"基因序列:"<<ga_s<<std::endl;
                std::cout<<"Ga_Mincost:"<<ga_minicost<<std::endl;

                return true;
            }

            mate_server(*population, *buffer);		// 个体基因交换
            swap(population, buffer);		// 交换父子群体
        }

        return 0;
    }
};


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


#endif //CDN_GA_H