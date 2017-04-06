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

#ifndef CDN_GA2_H
#define CDN_GA2_H


#define MAX_COST 1e8
// 85000000
#define TIME_END 88000000


// 遗传算法实现函数
class OurGA2 {
private:
    // 默认初始化参数

    // 种群基本参数
    int ga_max_iterate = 32000;	// 最大迭代次数 16384
    int ga_size; // 种群大小 2048

    // 初始化优秀基因与劣等基因变异率
    double ga_init_good_rate = 0.20f;
    double ga_init_bad_rate = 0.05f;
    double ga_line_one_minicost = INF;
    int ga_line_one_middle = -1;
    double ga_line_two_minicost = INF;
    int ga_line_two_middle = -1;

    // 用于字符串对比的vector数组
    // 全局变量
    std::vector<string> str_memory;
    int memory_g_steps = 50;
    int memory_size_g; // 不同等级的基因长度，其大小应不同，越长越不能记忆太多数量

    float ga_elitism_rate = 0.25f; // 精英比率 0.10f
    int decay_e_step = 1; // 多少步进行精英增加以及变异率增加 10
    double decay_e_rate;  // 不同等级基因大小的群体的衰减率
    int esize; // 精英在群体中的数量ga_size*ga_elitism_rate_now

    double decay_m_rate = 0.99;
    int mutate_step = 1; // 多少步后不同阶层个体突变情况的变化 10
    float ga_mutation_rate  = 0.25f; // 变异率 0.25f
    float ga_mutation; // 基因变异码

    int init_mutation_nums = 50; // 初始基因点位可变异数目
    int min_mutation_nums = 2;
    int decay_mutation_ga = 1;

    // 突变与精英率每次衰减率值
    float decay_rate = 0.05;

    // 全局步骤
    int ga_step;

    // 等级演化初始分布
    // 正向递减high=30--->high_line=10;middle=70+++>middle_line=90.所以中间模糊基因段变异概率会慢慢变大
    int high = 30;
    int middle = 70;
    int high_line = 10;
    int middle_line = 90;
    // 正向递减high=10+++>high_line=35;middle=90--->middle_line=65.所以中间模糊基因段变异概率会慢慢变小
    bool hm_flag = false;
    int hm_line_high = 35;
    int hm_line_middle = 65;
    int step_begin = 0;
    int steps_stop = 1;

    // 保存本次迭代最优基因及其适应度
    std::string ga_s;
    double ga_minicost = INF;

    // 写结果文件
    char *ga_filename;

    // 网络相关的动态初始化参数
    std::vector<int> ga_server = ChooseServer::serverID; // 工作原码
    int ga_target_size = ChooseServer::serverID.size(); // 工作基因长度

    // 基因片段不同等级划分
    // ServerID
    std::vector<int> ga_Id_server;
    int size_Id_server;
    // ServerCandidate
    std::vector<int> ga_Candidate_server;
    int size_Candidate_server;
    // ServerPossible
    std::vector<int> ga_possible_server;
    int size_Possible_server;

    // 求解最小费用流
    //ZKW ga_run;
    //MCMF ga_run;
    WeFastMCMF ga_run;

    // 遗传算法结构体，包括适应与最优解
    struct ga_struct {
        std::string str; // 个体基因编码串
        double fitness; // 个体适应度
    };
    typedef std::vector<ga_struct> ga_vector;// 合并写法

public:
    bool bSolve;

    // ----已测试----
    // 将下标对应到serverID
    std::vector<int> to_serverID(std::vector<int> tmp_serverID, int start, int end) {
        std::vector<int> done_serverID;

        for (int i = start; i < end; ++i) {
            done_serverID.push_back(tmp_serverID[i]);
        }

        return done_serverID;
    }

    // ----已测试----
    // 第一条线的二分搜索
    int binary_search_one(std::vector<int> bs_serverID, int low, int high, double *bs_minicost) {
        // 用于记录最小费用
        std::vector<int> find_line_bs;
        double minicost_bs;

        // 如果无法找到解，那么默认为(high+low)/2
        int i = 0; // 用于提前终止二维搜索
        int real_middle = -1; // 在没有查找到时为-1

        // 开始查找
        while(low<=high){
            //中间位置计算,low+最高位置减去最低位置,右移一位,相当于除2.也可以用(high+low)/2
            int middle=low + ((high-low)>>1);

            // 二维搜索
            if(i==11) {
                // 最多二分十次
                *bs_minicost = ga_line_one_minicost;

                return real_middle;
            }

            //计算此时的耗费
            std::vector <int>().swap(find_line_bs);
            find_line_bs = to_serverID(bs_serverID, 0, middle);
            minicost_bs = INF;
            minicost_bs = ga_run.run(Graph::nodeCount,Graph::arcCount, find_line_bs);
            if(minicost_bs < INF) {
                // 有解，向左搜索
                high=middle-1;

                // 记录查找找到的位置
                real_middle = find_line_bs.size();

                // 将查找过程中最小的耗费及服务器vector保存到全局耗费
                if (ga_line_one_minicost > minicost_bs) {
                    ga_line_one_minicost = minicost_bs;
                    ga_line_one_middle = find_line_bs.size();
                }
            }else {
                // 无解，向右搜索
                low=middle+1;
            }

            // 迭代次数统计
            i++;
        }

        // 迭代资源未用完，代表已精确定位
        if (i < 11) {
            // 精确搜索到
            *bs_minicost = ga_line_one_minicost;

            return real_middle;
        }
    }

    // ----已测试----
    // 第二条线的二分搜索
    // 此处需要做边界处理，当minicost_bs的值均为小于或大于one_minicost时取（candidate_size+possible_size）/2
    int binary_search_two(std::vector<int> bs_serverID, double one_minicost, int low, int high) {
        // 将查找第一分线发现的最小耗费赋值给ga_line_two_minicost
        ga_line_two_minicost = INF;

        std::vector<int> find_line_bs;
        double minicost_bs = INF;
        int real_middle = -1; //如果无法找到解，那么默认为(high+low)/2

        // 用于提前终止二维搜索
        int i = 0;

        while(low<=high){
            //中间位置计算,low+最高位置减去最低位置,右移一位,相当于除2.也可以用(high+low)/2
            int middle=high - ((high-low)>>1);

            // 二维搜索
            if(i==11) {
                // 最多二分八次
                return real_middle;
            }

            //计算此时的耗费
            find_line_bs = to_serverID(bs_serverID, middle, bs_serverID.size());
            minicost_bs = ga_run.run(Graph::nodeCount,Graph::arcCount, find_line_bs);
            if(minicost_bs < INF) {
                // 有解，向右搜索进行逼近
                low=middle+1;

                // 将查找过程中最小的耗费及分割点保存到全局耗费
                if (ga_line_two_minicost > minicost_bs) {
                    ga_line_two_minicost = minicost_bs;
                    ga_line_two_middle = find_line_bs.size();
                }
            }else {
                // 无解，向左搜索逼近
                high=middle-1;
                real_middle = high+1;
            }

            // 迭代次数统计
            i++;
        }

        // 迭代资源未用完，代表已精确定位
        if (i < 11) {
            // 精确搜索到
            return real_middle;
        }
    }

    // ----已测试----
    // 找分界线
    bool finding_boundary() {
        // 初始化临时变量
        // ServerID
        std::vector<int> ga_wk_tmp = ChooseServer::serverID;
        int size_Id_tmp = ChooseServer::serverID.size();
        // ServerCandidate
        ga_wk_tmp.insert(ga_wk_tmp.end(),
                         ChooseServer::serverCandidate.begin(), ChooseServer::serverCandidate.end());
        int size_Candidate_tmp = ChooseServer::serverCandidate.size();
        // ServerPossible
        ga_wk_tmp.insert(ga_wk_tmp.end(),
                         ChooseServer::serverPossible.begin(), ChooseServer::serverPossible.end());
        int size_Possible_tmp = ChooseServer::serverPossible.size();

        //*******************************第一分线****************************************//
        // 从高到低找第一分线
        int size_find_line_one = -1;
        // 计算初始serverID耗费
        double minicost_tmp_one = INF;

        // 查找第一分线位置
        size_find_line_one = binary_search_one(ga_wk_tmp, 0, ga_wk_tmp.size(), &minicost_tmp_one);

        // 开始寻找第一条线
        if (size_find_line_one == -1) {
            std::cout<<"No LP solve!\n"<<std::endl;

            return false;
        }

        //*******************************第二分线****************************************//
        // 开始寻找第二条线
        //
        //注意：第一条线找到的serverID可能出现minicost=INF的情况（暂时不管）
        //
        double minicost_tmp_two = INF;
        if (minicost_tmp_one < INF) {
            minicost_tmp_two = minicost_tmp_one;
        }
        else {
            std::cout<<"ServerID's minicost is INF!"<<std::endl;
        }

        // 用于保存第二条线的serverID和数组索引值
        int size_find_line_two;

        // 开始第二条线的二分一维搜索
        size_find_line_two = binary_search_two(ga_wk_tmp, minicost_tmp_two, 0, ga_wk_tmp.size());

        // 做边界处理
        if (size_find_line_two == -1) {
            std::cout<<"No LP solve!\n"<<std::endl;
            return false;
        }
        else {
            if (size_find_line_two < size_find_line_one) {
                int tmp_line_one = size_find_line_one;

                size_find_line_one = size_find_line_two;
                size_find_line_two = tmp_line_one;
            }
        }

        // 将第一条线的元素还原
        // serverID
        std::vector <int>().swap(ChooseServer::serverID);
        for (int i = 0; i < size_find_line_one; ++i) {
            ChooseServer::serverID.push_back(ga_wk_tmp[i]);
        }

        // 将第二条线的元素还原serverPossible
        //serverCandidate
        std::vector <int>().swap(ChooseServer::serverCandidate);
        for (int i = size_find_line_one; i < size_find_line_two; ++i) {
            ChooseServer::serverCandidate.push_back(ga_wk_tmp[i]);
        }

        //serverPossible
        std::vector <int>().swap(ChooseServer::serverPossible);
        for (int j = size_find_line_two; j < ga_wk_tmp.size(); ++j) {
            ChooseServer::serverPossible.push_back(ga_wk_tmp[j]);
        }

        // 基因片段不同等级划分
        // ServerID
        ga_Id_server = ChooseServer::serverID;
        size_Id_server = ChooseServer::serverID.size();
        // ServerCandidate
        ga_Candidate_server = ChooseServer::serverCandidate;
        size_Candidate_server = ChooseServer::serverCandidate.size();
        // ServerPossible
        ga_possible_server = ChooseServer::serverPossible;
        size_Possible_server = ChooseServer::serverPossible.size();

        return true;
    }

    // ----已测试----
    // 初始化化参数
    OurGA2(char *filename) {
        std::cout<<"使用GA默认初始参数！"<<std::endl;

        // 初始化种群大小
        if (ga_target_size < 100) {
            ga_size = 100;
        }
        else if ((ga_target_size >= 100) && (ga_target_size < 200)) {
            ga_size = 64;
        }
        else if ((ga_target_size >= 200) && (ga_target_size < 300)) {
            ga_size = 36;
        }
        else if ((ga_target_size >= 300) && (ga_target_size < 400)) {
            ga_size = 9;
        }
        else {
            ga_size = 9;
        }

        // 同等级基因大小的群体的衰减率
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

        // 不同等级的基因长度，其大小应不同，越长越不能记忆太多数量
        if (ga_target_size < 100) {
            memory_size_g = 1440;
        }
        else if ((ga_target_size >= 100) && (ga_target_size < 200)) {
            memory_size_g = 1000;
        }
        else if ((ga_target_size >= 200) && (ga_target_size < 300)) {
            memory_size_g = 250;
        }
        else if ((ga_target_size >= 300) && (ga_target_size < 400)) {
            memory_size_g = 49;
        }
        else {
            memory_size_g = 25;
        }

        // 精英在群体中的数量ga_size*ga_elitism_rate_now
        esize = ceil(ga_size * ga_elitism_rate);

        // 基因变异码
        ga_mutation = RAND_MAX * ga_mutation_rate;

        // 写结果文件
        ga_filename = filename;

        // 二分一维搜索重新确定LP的两条线
        finding_boundary();

        // 网络相关的动态初始化参数
        ChooseServer::serverID.insert(ChooseServer::serverID.end(),
                                      ga_Candidate_server.begin(), ga_Candidate_server.end());
        ChooseServer::serverID.insert(ChooseServer::serverID.end(),
                                      ga_possible_server.begin(), ga_possible_server.end());
        ga_target_size = ChooseServer::serverID.size();
        ga_server = ChooseServer::serverID;

        std::cout<<"GA length:"<<ga_target_size<<std::endl;

        //ZKW ga_run;
//        if (ga_run.run(Graph::nodeCount,Graph::arcCount, ChooseServer::serverID) >= INF) {
//            printf("No LP Solve!\n");
//
//            bSolve = false;
//        }
//        else {
//            write_result(ga_run.getRoute(), ga_filename);
//            bSolve = true;
//        }
    }

    // ----已测试----
    // 初始化函数
    // 备注，暂时不用
    OurGA2(int size, int max_iterate, float elitism_rate, float mutation_rate) {
        ga_size = size;
        ga_max_iterate = max_iterate;
        ga_elitism_rate = elitism_rate;
        ga_mutation_rate = mutation_rate;
    }

    // ----已测试----
    // 初始化阶段，优秀基因段变异概率
    int init_good_design() {
        int init_good = RAND_MAX * ga_init_good_rate;
        if (rand() < init_good) {
            return 0;
        }
        else {
            return 1;
        }
    }

    // ----已测试----
    // 初始化阶段，劣等基因段变异概率
    int init_bad_design() {
        int init_bad = RAND_MAX * ga_init_bad_rate;
        if (rand() < init_bad) {
            return 1;
        }
        else {
            return 0;
        }
    }

    // ----已测试----
    // 向str_memory中写入string字符串
    bool insert_string_memory(std::string tmp_string) {
        if (str_memory.size() < memory_size_g) {
            // vector数组未满，直接push_back
            str_memory.push_back(tmp_string);

            return true;
        }
        else {
            // 用于记录随机删除的位置
            int rand_remove;
            vector<string>::iterator iter_delete = str_memory.begin();

            // 随机删除一个vector元素
            rand_remove = rand() % memory_size_g;
            iter_delete += rand_remove;
            str_memory.erase(iter_delete);

            // push_back基因字符串
            str_memory.push_back(tmp_string);
        }
    }

    // ----已测试----
    // 用于确定该基因序列是否已经产生过
    bool compare_string(std::string tmp_string) {
        bool b_equal = false;

        // 进行对比
        for (int i = 0; i < str_memory.size(); ++i) {
            if (str_memory[i].compare(tmp_string) == 0) {
                b_equal = true;

                break;
            }
        }

        // 返回对比结果，true为存在
        return b_equal;
    }

    // ----已测试----
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

                // 将基因存入str_memory
                insert_string_memory(citizen.str);
            }
            else if (i == 1) {
                // 将最优个体基因加入种群
                for (int j=0; j<size_Id_server; j++) {
                    citizen.str += std::to_string(1);
                }
                for (int k=size_Id_server; k<ga_target_size; k++) {
                    citizen.str += std::to_string(1);
                }

                ga_s.clear();
                ga_s = citizen.str;

                // 将基因存入str_memory
                insert_string_memory(citizen.str);
            }
            else {
                // 初始化其余个体基因
                // 对基因序列进行分段初始化
                for (int j=0; j<ga_target_size; j++) {
                    if (j >= 0 && j < size_Id_server) {
                        // 第一段0～serverID.size(), 90%为1
                        citizen.str += std::to_string(init_good_design());
                    }
                    else if (j >= size_Id_server && j < (size_Id_server + size_Candidate_server)) {
                        // 第三段serverCandidate.seze()+1~serverPossible.size(), 90%为0
                        citizen.str += std::to_string(rand() % 2);
                    }
                    else {
                        // 第二段serverID.size()+1~serverCandidate.size(),0与1一半一半
                        citizen.str += std::to_string(init_bad_design());
                    }
                }

                // 将基因存入str_memory
                // 因为是随机初始化，可以大致认为每个个体的基因序列均不相同
                insert_string_memory(citizen.str);
            }

            // 将个体放入种群
            population.push_back(citizen);
        }

        // 分配一个存储空间
        buffer.resize(ga_size);

        //PrintGA(population);
    }

    // ----已测试----
    // 计算适应度（服务器）
    void calc_fitness_server(ga_vector &population, int step) {
        // 查看种群个体基因序列
        //PrintGA(population);

        // 第一次计算适应度精英数未知
        int tmp_esize;
        if (step == 0) {
            tmp_esize = 0;
        }
        else {
            tmp_esize = esize;
        }

        for (int i=tmp_esize; i<ga_size; i++) {
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
                population[i].fitness = ga_run.run(Graph::nodeCount,Graph::arcCount,ChooseServer::serverID);
            }
            else {
                // 没有服务节点的个体
                population[i].fitness = INF;
            }
        }
    }

    // ----已测试----
    // 根据适应度对个体进行排序，从小到大排序
    static bool fitness_sort(ga_struct x, ga_struct y) {
        return (x.fitness < y.fitness);
    }
    inline void sort_by_fitness(ga_vector &population) {
        std::sort(population.begin(), population.end(), fitness_sort);
    }

    // ----已测试----
    // 突变概率设计,不同阶层在不同时期的突破概率不同
    int mutate_design() {
        // ——————待测试——————
        int g = rand() % 100;

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

    // ----已测试----
    // 突变操作（服务器）
    void mutate_server(ga_struct &member) {
        int choose;
        int ipos;

        // 每运行步数衰减变异数目
        if (ga_step % decay_mutation_ga == (decay_mutation_ga - 1))
        {
            if (init_mutation_nums > 1) {
                init_mutation_nums -= 1;
            }
        }

        // 重复变异几次，提高成功率，且变异数会慢慢变小
        for (int i = 0; i < init_mutation_nums; ++i) {
            choose = mutate_design();
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
                    ipos = rand() % (size_Id_server + size_Candidate_server);
                }
            }

            (member.str[ipos] == '1') ? (member.str[ipos] = '0') : (member.str[ipos] = '1');
        }
    }

    // ----已测试----
    // 精英群体
    void elitism(ga_vector &population, ga_vector &buffer, int esize) {
        for (int i=0; i<esize; i++) {
            buffer[i].str = population[i].str;
            buffer[i].fitness = population[i].fitness;
        }
    }

    // ----已测试----
    // 交换操作（服务器）
    void mate_server(ga_vector &population, ga_vector &buffer) {
        int spos;
        int i1;
        int i2;

        // 精英群体（遗传）
        elitism(population, buffer, esize);

        // 与其它个体基因进行变异
        int memory_l_step = 0;
        for (int i=esize; i<ga_size; i++) {
            i1 = rand() % ga_size;
            i2 = rand() % ga_size;

            // ----已测试----
            // 基因交换(拼接)
            spos = rand() % ga_target_size;
            buffer[i].str = population[i1].str.substr(0, spos) +
                            population[i2].str.substr(spos, ga_target_size - spos);
            buffer[i].fitness = INF;

            // 基因位变异
            if (rand() < ga_mutation) mutate_server(buffer[i]);

            // ----已测试----
            // 将突变个体基因存入str_memory
            if (!compare_string(buffer[i].str)) {
                insert_string_memory(buffer[i].str);
            }
            else {
                // 需做边界处理，以防出现死循环；定义全局变量memory_g_steps,大于此参数，将把这个参数赋给buffer
                --i;

                memory_l_step += 1;
                if (memory_l_step > memory_g_steps) {
                    insert_string_memory(buffer[i].str);
                }
            }
        }
    }

    // ----已测试----
    // 打印输出本次迭代最好的个体
    inline void print_best(ga_vector &gav) {
        std::cout << "Best: " << gav[0].str << " (" << gav[0].fitness << ")" << std::endl;
        ga_s.clear();
        ga_s = gav[0].str;
        ga_minicost = gav[0].fitness;
    }

    // ----已测试----
    // 基因解码
    // ----已测试----
    // 基因解码
    void decode() {
        // 清空服务器节点放置缓存
        std::vector <int>().swap(ChooseServer::serverID);

        for (int i = 0; i < ga_s.size(); ++i) {
            if (ga_s[i] == '1') {
                ChooseServer::serverID.push_back(ga_server[i]);
            }
        }

        if (!ChooseServer::serverID.empty()) {
            //WeMCMF1 ga_run1;

            if (ga_run.run(Graph::nodeCount,Graph::arcCount, ChooseServer::serverID) < INF) {
                write_result(ga_run.getRoute(),ga_filename);
            }

            //ga_run1.clearData(ga_run1.s);
        }
        else {
            std::cout<<"Code is Error Error, Please check it!\n"<<std::endl;
        }
    }

    // ----已测试----
    // population等级概率渐变
    void population_change() {
        // 循环突变
        if (hm_flag) {
            // 第二次调整，中间基因突变变大
            if (ga_step % mutate_step == (mutate_step - 1)) {
                if (high < hm_line_high) {
                    high += 1;
                }
                if (middle > hm_line_middle) {
                    middle -= 1;
                }
            }

            // 未测试
            if (high == hm_line_high && middle == hm_line_middle) {
                step_begin += 1;

                if (step_begin >= steps_stop) {
                    hm_flag = false;
                    step_begin = 0;
                }
            }
        }
        else {
            // 第一次调整，中间基因突变变大
            if (ga_step % mutate_step == (mutate_step - 1)) {
                if (high > high_line) {
                    high -= 1;
                }
                if (middle < middle_line) {
                    middle += 1;
                }
            }

            // 未测试
            if (high == high_line && middle == middle_line) {
                step_begin += 1;
                if (step_begin >= steps_stop) {
                    hm_flag = true;

                    step_begin = 0;
                }
            }
        }
    }

    // ----已测试----
    // 精英率与变异率递增
    void decay(int step) {
        if ((step % decay_e_step) == (decay_e_step - 1)) {
            // 精英率递增
            if (ga_elitism_rate * (1 + decay_rate) < decay_e_rate) {
                ga_elitism_rate *= 1 + decay_rate;
            }
            // 将精英率转变为种群中精英个体数目
            esize = ceil(ga_size * ga_elitism_rate);

            // 变异率递增
            if (ga_mutation_rate * (1 + decay_rate) < decay_m_rate) {
                ga_mutation_rate *= 1 + decay_rate;
            }
            // 将变异率转变为real突变数
            ga_mutation = RAND_MAX * ga_mutation_rate;
        }
    }

    // ----已测试----
    // 交换父子群体
    void swap(ga_vector *&population, ga_vector *&buffer) {
        ga_vector *temp = population;
        population = buffer;
        buffer = temp;
    }

    // ----已测试----
    // 打印基因序列
    void PrintGA(ga_vector &population) {
        for (int i=0; i<ga_size; i++) {
            std::cout<<(population)[i].str<<"\t"<<(population)[i].fitness<<std::endl;
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
        int end_steps = 0;
        double tmp_min_minicost=INF;
        for (int i=0; i<ga_max_iterate; i++) {
            ga_step = i;

            // population等级概率渐变
            population_change();

            // 精英率与变异率衰减
            decay(ga_step);

            // 用于统计迭代minicost重复的次数
            tmp_min_minicost = (*population)[0].fitness;

            // 计算适应度
            calc_fitness_server(*population, i);

            // 对个体进行排序
            sort_by_fitness(*population);

            // 用于统计迭代minicost重复的次数
            if ((*population)[0].fitness == tmp_min_minicost) {
                end_steps += 1;
            }
            else {
                end_steps = 0;
            }

            // 输出最好的个体
            print_best(*population);

            // ----已测试----
            if (((clock() - t0) > TIME_END) || (i == ga_max_iterate-1) || end_steps == 800) {
                decode();  // 基因解码
                std::cout<<"基因序列:"<<ga_s<<std::endl;
                std::cout<<"Ga_Mincost:"<<ga_minicost<<std::endl;

                return true;
            }

            // 个体基因交换
            mate_server(*population, *buffer);

            // 交换父子群体
            swap(population, buffer);
        }

        return 0;
    }
};



///**
// * 存储任意两点之间的距离和路径，Floyd算法
// * */
//int VertexCost[MAXNODE][MAXNODE];
//int VertexPath[MAXNODE][MAXNODE];
//std::vector<int> getPath;
//
//
//struct GA {
//    // Floyd计算任意两点之间的距离
//    int ComputeCost(Arc *gMatrix[][MAXNODE], int vexNum);
//
//    // 递归寻路
//    void  Prn_Pass(int j , int k);
//    // 获取两点之间的路径
//    bool GetPath(int i, int j);
//};
//
//
//int GA::ComputeCost(Arc *gMatrix[][MAXNODE], int vexNum) {
//    for (int i = 0;i < vexNum;i++) {
//        for (int j = 0;j < vexNum;j++) {
//            if (i == j) {
//                VertexCost[i][j] = 0;
//            }
//            else if(gMatrix[i][j] != nullptr) {
//                VertexCost[i][j] = (gMatrix[i][j]->capacity) * (gMatrix[i][j]->cost);
//            }
//            else {
//                VertexCost[i][j] = MAX_COST;
//            }
//            VertexPath[i][j] = -1;
//        }
//    }
//
//    //关键代码部分
//    for (int k = 0;k < vexNum;k++) {
//        for (int i = 0;i < vexNum;i++) {
//            for (int j = 0;j < vexNum;j++) {
//                if (VertexCost[i][k] + VertexCost[k][j] < VertexCost[i][j]) {
//                    VertexCost[i][j] = VertexCost[i][k] + VertexCost[k][j];
//                    VertexPath[i][j] = k;
//                }
//            }
//        }
//    }
//
//    return 0;
//}
//
//// 获取两点之间的路径
//void  GA::Prn_Pass(int j , int k) {
//    if (VertexPath[j][k]!=-1) {
//        Prn_Pass(j,VertexPath[j][k]);
//        std::cout<<"-->"<<VertexPath[j][k];
//        getPath.push_back(VertexPath[j][k]);
//        Prn_Pass(VertexPath[j][k],k);
//    }
//}
//
//bool GA::GetPath(int i, int j) {
//    std::cout<<i<<"到"<<j<<"的最短路径为:";
//    std::cout<<i;
//    Prn_Pass(i, j);
//    std::cout<<"-->"<<j<<std::endl;
//    std::cout<<"最短路径长度为:"<<VertexCost[i][j]<<std::endl;
//
//    if (!getPath.empty()) {
//        return false;
//    }
//
//    return true;
//}


#endif //CDN_GA2_H