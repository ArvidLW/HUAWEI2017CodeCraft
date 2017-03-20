//
// Created by lw_co on 2017/3/20.
//

#ifndef MYLP_MYLP_H
#define MYLP_MYLP_H


#include <iostream>
#include <vector>
enum LinearProgrammingResultKind{
    NO_FEASIBLE_ANSWER=0,
    NO_BOUNDED_ANSWER,
    INFINITE_ANSWER,
    UNIQUE_OPITMAL_ANSWER
} ;
struct LinearProgrammingResult{
    double cost;
    std::vector<double> valueList;
    LinearProgrammingResultKind kind;
    LinearProgrammingResult(int n=0){
        if(n<0)
            n=0;
        valueList.resize(n);
    }
    void print(){
        std::cout<<"Optimal Cost:"<<cost<<std::endl;
        std::cout<<"Value of each variable:"<<std::endl;
        for(int i=0;i<(signed)valueList.size();i++){
            std::cout<<"x"<<(i+1)<<"="<<valueList.at(i)<<std::endl;
        }
    }
};
extern LinearProgrammingResult linearPSimplexM(std::vector<std::vector<double> >& matrixA,
                                                                     std::vector<double>& vectorC);
extern void testLinearP();


#endif //MYLP_MYLP_H
