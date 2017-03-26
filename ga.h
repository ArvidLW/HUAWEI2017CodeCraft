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

    if (getPath.empty() == FALSE) {
        return FALSE;
    }

    return TRUE;
}


// 遗传算法实现函数
class OurGA {
private:
    int ga_size = 2048; // 种群大小
    int ga_max_iterate = 16384;	// 最大迭代次数
    float ga_elitism_rate = 0.10f; // 精英比率
    float ga_mutation_rate  = 0.25f; // 变异率
    float ga_mutation = RAND_MAX * ga_mutation_rate;
    std::string ga_target = "1110001111011111100001111100"; // 目标

    // 遗传算法结构体，包括适应与最优解
    struct ga_struct {
        string str;						// the string
        unsigned int fitness;			// its fitness
    };
    typedef vector<ga_struct> ga_vector;//

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

    // 初始化种群
    void init_population(ga_vector &population, ga_vector &buffer ) {
        int tsize = ga_target.size(); // 目标大小，即最优服务器可能包含的数量

        for (int i=0; i<ga_max_iterate; i++) {
            ga_struct citizen;

            citizen.fitness = 0;
            citizen.str.erase();

            // 初始化个体基因
            for (int j=0; j<tsize; j++) {
                citizen.str += std::to_string(rand() % 2);
            }
            std::cout<<citizen.str<<std::endl;

            // 将个体放入种群
            population.push_back(citizen);
        }

        // 分配一个存储空间
        buffer.resize(ga_size);
    }

    // 计算适应度
    void calc_fitness(ga_vector &population) {
        string target = ga_target;
        int tsize = target.size();
        unsigned int fitness;

        for (int i=0; i<ga_size; i++) {
            fitness = 0;
            for (int j=0; j<tsize; j++) {
                fitness += abs(int(population[i].str[j] - target[j]));
            }

            population[i].fitness = fitness;
        }
    }

    // 从小到大排序
    static bool fitness_sort(ga_struct x, ga_struct y) {
        return (x.fitness < y.fitness);
    }
    // 根据适应度对个体进行排序
    inline void sort_by_fitness(ga_vector &population) {
        sort(population.begin(), population.end(), fitness_sort);
    }

    // 精英群体
    void elitism(ga_vector &population, ga_vector &buffer, int esize) {
        for (int i=0; i<esize; i++) {
            buffer[i].str = population[i].str;
            buffer[i].fitness = population[i].fitness;
        }
    }

    // 突变操作
    void mutate(ga_struct &member) {
        int tsize = ga_target.size();
        int ipos = rand() % tsize;
        int delta = (rand() % 2);

        member.str[ipos] = ((member.str[ipos] + delta) % 2);
    }

    // 交换操作
    void mate(ga_vector &population, ga_vector &buffer) {
        int spos;
        int i1;
        int i2;
        int esize = ga_size * ga_elitism_rate;
        int tsize = ga_target.size();

        // 精英群体
        elitism(population, buffer, esize);

        // 与其它个体基因进行交换
        for (int i=esize; i<ga_size; i++) {
            i1 = rand() % (ga_size / 2);
            i2 = rand() % (ga_size / 2);
            spos = rand() % tsize;

            buffer[i].str = population[i1].str.substr(0, spos) +
                            population[i2].str.substr(spos, esize - spos);

            if (rand() < ga_mutation) mutate(buffer[i]);
        }
    }

    // 打印输出本次迭代最好的个体
    inline void print_best(ga_vector &gav) {
        std::cout << "Best: " << gav[0].str << " (" << gav[0].fitness << ")" << std::endl;
    }

    // 交换父子群体
    void swap(ga_vector *&population, ga_vector *&buffer) {
        ga_vector *temp = population;
        population = buffer;
        buffer = temp;
    }

    bool GaAlgorithm(std::vector<int> InitialServer, std::vector<int> AlternataServer,
                    int vertexNum, int edgeNum) {
        // 用于产生伪随机数的时间种子
        srand(unsigned(time(NULL)));

        ga_vector pop_alpha, pop_beta;
        ga_vector *population, *buffer;

        init_population(pop_alpha, pop_beta);
        population = &pop_alpha;
        buffer = &pop_beta;

        for (int i=0; i<ga_max_iterate; i++) {
            calc_fitness(*population);		// 计算适应度
            sort_by_fitness(*population);	// 对个体进行排序
            print_best(*population);		// 输出最好的个体

            if ((*population)[0].fitness == 0) break;

            mate(*population, *buffer);		// 个体基因交换交换
            swap(population, buffer);		// 交换父子群体
        }

        return 0;
    }
};


#endif //CDN_GA_H
