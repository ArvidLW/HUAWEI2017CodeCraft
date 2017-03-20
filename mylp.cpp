//
// Created by lw_co on 2017/3/20.
//
#include <cmath>
#include "mylp.h"
//优化：http://www.doc88.com/p-2116741440818.html
/**
算法设计：
主要参考《单纯形法原理讲解.ppt》(http://wenku.baidu.com/link?url=2RU-il2OGzmhDo8a9-J8wf_MAN_toNk3UKZhCVwSTa_g0BRWU32qWW_uddN7rsLuQImwbocK8QzoCpX9AYwWk3fTnSVAs1_-cl55OxBGHOG)
这篇文章，在该篇文章中，首先根据一个案例（在本程序中也使用它作为测试案例）讲解了利用单
纯形法来求解线性规划问题；然后在从理论上介绍了单纯形法，并给出了一个迭代算法。
但是上述给出的算法极其模糊、简略。

所以，基于此，我设计了如下的算法：
一些量：n为所有变量的个数，m是该线性规划中约束条件不等式的个数，矩阵A是系数矩阵和b的合体，
矩阵A的最前面是加入的松弛变量的系数，故此为一个m*m的单位矩阵。c则是最优化式中各变量对应的
系数组成的一个列向量。由于矩阵A的最前面的松弛变量的系数，故C的前m个元素都为0，因为松弛量
不对最优化值做贡献。
1. 检查各个非基量的校验数（其实就是Ci,i=m+1,m+2,...,n），根据《单纯形法原理讲解.ppt》文章
	中给出的定理，判断出该线性规划问题的解的情况——存在唯一解、存在无穷多解、无界解。目前无
	可行解（涉及到人工变量，即为了处理“≥约束条件”而添加的变量）尚未考虑，以后再改进吧。
	具体的判断过程与原理请看代码和《单纯形法原理讲解.ppt》。
2. 换基
2.1 找出换入量——inI。
2.2 找出换出量——outI。
2.3 更新A：利用矩阵行变换将第inI列变为第outI列的内容，即变为一个单位列向量，第outI个元素为1。
2.3 更新费用：cost=cost+C[inI]*A[outI][n]。
2.4 更新向量C：C=C-C[inI]*A[outI]。
2.5 交换矩阵A：第inI列与第outI列交换
2.6 交换向量c：第inI元素与第outI元素交换
2.7 保存原序列顺序。然后转第1步继续执行。

其他参考文章：
《线性规划的基本算法》(http://www.doc88.com/p-707862483794.html)：该篇文章介绍了单纯形法，
	理论性较强，但在操作性方面讲解较少。
《线性规划 》(http://blog.sina.com.cn/s/blog_61e8042b0100eepi.html)：该篇文章相当于线
	性规划方面的一个简介。
《运筹学--单纯形法求解-动态演示.ppt》(http://wenku.baidu.com/link?url=hzL2zfm4R-eMljnUH1V4WUrFzWjaaJdfHIRKqN2A4Ojl2OXORx9PjhDG_SXrbC5T2c7a6pVy5TFluhBwBwO-l47yoHEMhCBOyonfO_tXjv7)：
	该篇文章主要给出了利用一个表格(他/她称为单纯形表)，不过我没怎么看懂，想看的就看一下
	吧，貌似不错的样子。
*/
using namespace std;
static const double DOUBLE_MIN=1e-8;

/**
该函数利用单纯形法来求解线性规划，并且目前只考虑“≤约束条件”（加入松弛变量即形成可行基），不考虑
“≥约束条件”（加入非负人工变量）。
matrixA：这是一个矩阵的，前面各列保存各个x对应的系数矩阵，最后的一列则保存b。另外，将松弛变量
	对应的系数放在最前面，所以该矩阵的最前面是一个m*m的方阵（假设该线性规划共有m个约束条件不等式）。
*/
LinearProgrammingResult linearPSimplexM(vector<vector<double> >& matrixA,vector<double>& vectorC){
    int m=matrixA.size();//方程的个数
    int n=vectorC.size();
    LinearProgrammingResult result(n-m);
    if(m<=0||n<=0||(signed)matrixA.at(0).size()-1!=n){
        result.kind=LinearProgrammingResultKind::NO_FEASIBLE_ANSWER;
        return result;
    }

    int optimalValue=0;
    vector<int> positionList(n);
    for(int i=0;i<n;i++)
        positionList.at(i)=i;
    while(true){
        //1. 检测检验数，判断该线性规划的解的情况;
        vector<int> checkAboveZeroList;
        for(int i=m;i<n;i++){
            if(vectorC.at(i)>0)
                checkAboveZeroList.push_back(i);
        }
        if(checkAboveZeroList.size()==0){
            bool isUnique=true;
            for(int i=m;i<n;i++)
                if(fabs(vectorC.at(i))<DOUBLE_MIN){
                    isUnique=false;
                    break;
                }

            if(isUnique){ //存在唯一解
                result.kind=LinearProgrammingResultKind::UNIQUE_OPITMAL_ANSWER;
                result.cost=optimalValue;
                //根据positionList中存储的系数与系数直接的对应关系，设置变量的真实系数，
                //	系数值要么是矩阵A的最后一列b中的某一个（基量），要么就是0（非基量）。
                for(int i=0;i<n;i++){
                    if(positionList.at(i)>=m){
                        result.valueList.at(positionList.at(i)-m)=(i<m?matrixA[i][n]:0);
                    }
                }
            }else{//存在无穷多解
                result.kind=LinearProgrammingResultKind::INFINITE_ANSWER;
            }
            return result;
        }else{
            int inI{0},outI{0};
            double max;
            bool isLimited;
            for(int i=0;i<(signed)checkAboveZeroList.size();i++){
                double min=0;
                bool isFound=false;
                isLimited=false;
                if(i==0){//为了给max进行初始化
                    max=vectorC.at(checkAboveZeroList.at(i));
                    //寻找需要换入和换出的基量
                    for(int j=0;j<m;j++){
                        if(!isFound&&matrixA[j][checkAboveZeroList.at(i)]>0&&matrixA[j][n]>=0){
                            isFound=true;
                            isLimited=true;
                            inI=checkAboveZeroList.at(i);
                            outI=j;
                            min=matrixA[j][n]/matrixA[j][checkAboveZeroList.at(i)];
                            continue;
                        }
                        if(isFound&&matrixA[j][checkAboveZeroList.at(i)]>0&&matrixA[j][n]>=0&&
                           matrixA[j][n]/matrixA[j][checkAboveZeroList.at(i)]<min){
                            inI=checkAboveZeroList.at(i);
                            outI=j;
                            min=matrixA[j][n]/matrixA[j][checkAboveZeroList.at(i)];
                        }
                    }

                }else{//处理剩余的满足检验条件的列
                    if(vectorC.at(checkAboveZeroList.at(i))<=max){//因为该检验数比最大值小，所以只做是否无界检测
                        for(int j=0;j<m;j++){
                            if(!isFound&&matrixA[j][checkAboveZeroList.at(i)]>0&&matrixA[j][n]>=0){
                                isLimited=true;
                                break;
                            }
                        }
                    }else{
                        max=vectorC.at(checkAboveZeroList.at(i));
                        for(int j=0;j<m;j++){
                            if(!isFound&&matrixA[j][checkAboveZeroList.at(i)]>0&&matrixA[j][n]>=0){
                                isFound=true;
                                isLimited=true;
                                inI=checkAboveZeroList.at(i);
                                outI=j;
                                min=matrixA[j][n]/matrixA[j][checkAboveZeroList.at(i)];
                                continue;
                            }
                            if(isFound&&matrixA[j][checkAboveZeroList.at(i)]>0&&matrixA[j][n]>=0&&
                               matrixA[j][n]/matrixA[j][checkAboveZeroList.at(i)]<min){
                                inI=checkAboveZeroList.at(i);
                                outI=j;
                                min=matrixA[j][n]/matrixA[j][checkAboveZeroList.at(i)];
                            }
                        }
                    }
                }
                if(!isLimited)
                    break;
            }
            if(!isLimited){//无界解
                result.kind=LinearProgrammingResultKind::NO_BOUNDED_ANSWER;
                return result;
            }
                //2. 将相应的基量和非基量交换
            else{
                //2.1 更新A：利用矩阵行变换将第inI列变为第outI列的内容，即变为一个单位列向量，第outI个元素为1
                if(fabs(matrixA[outI][inI])>DOUBLE_MIN){
                    for(int i=0;i<m;i++){
                        if(i==outI||fabs(matrixA[i][inI])<DOUBLE_MIN)
                            continue;
                        double div=matrixA[i][inI]/matrixA[outI][inI];
                        for(int j=0;j<=n;j++){//根据矩阵行变换，更新该行
                            matrixA[i][j]-=(div*matrixA[outI][j]);
                        }
                    }
                    //根据矩阵行变换将第outI行的第inI列元素置为1
                    if(fabs(matrixA[outI][inI]-1)>DOUBLE_MIN){
                        for(int j=0;j<=n;j++){//根据矩阵行变换，更新该行
                            if(j==inI)
                                continue;
                            matrixA[outI][j]/=matrixA[outI][inI];
                        }
                        matrixA[outI][inI]=1;
                    }
                }
                //2.2 更新费用
                optimalValue+=(vectorC.at(inI)*matrixA[outI][n]);
                //2.3 更新向量c
                if(fabs(vectorC.at(inI))>DOUBLE_MIN){
                    for(int i=0;i<n;i++){
                        if(i==inI)
                            continue;
                        vectorC.at(i)-=(vectorC.at(inI)*matrixA[outI][i]);
                    }
                    vectorC.at(inI)=0;
                }
                //2.4 交换矩阵A：第inI列与第outI列交换
                for(int i=0;i<m;i++){
                    double tmp=matrixA[i][inI];
                    matrixA[i][inI]=matrixA[i][outI];
                    matrixA[i][outI]=tmp;
                }
                //2.5 交换向量c：第inI元素与第outI元素交换
                double tmp=vectorC.at(inI);
                vectorC.at(inI)=vectorC.at(outI);
                vectorC.at(outI)=tmp;
                //2.6 保存原序列顺序
                int t=positionList.at(inI);
                positionList.at(inI)=positionList.at(outI);
                positionList.at(outI)=t;
            }
        }
    }
}

void testLinearP(){
    vector<vector<double> > matrix;
    vector<double> row;
    ////////////////////////////////////////////////
    /*
    案例真实结果：最优值为14，x1=4，x2=2。
    该案例取自《单纯形法原理讲解.ppt》(http://wenku.baidu.com/link?url=2RU-il2OGzmhDo8a9-J8wf_MAN_toNk3UKZhCVwSTa_g0BRWU32qWW_uddN7rsLuQImwbocK8QzoCpX9AYwWk3fTnSVAs1_-cl55OxBGHOG)
    */
    matrix.clear();

    row.clear();
    row.push_back(1);
    row.push_back(0);
    row.push_back(0);
    row.push_back(120);
    row.push_back(210);
    row.push_back(15000);
    matrix.push_back(row);

    row.clear();
    row.push_back(0);
    row.push_back(1);
    row.push_back(0);
    row.push_back(110);
    row.push_back(30);
    row.push_back(4000);
    matrix.push_back(row);

    row.clear();
    row.push_back(0);
    row.push_back(0);
    row.push_back(1);
    row.push_back(1);
    row.push_back(1);
    row.push_back(75);
    matrix.push_back(row);

    vector<double> c;
    c.push_back(0);
    c.push_back(0);
    c.push_back(0);
    c.push_back(143);
    c.push_back(60);

    /*row.clear();
    row.push_back(1);
    row.push_back(0);
    row.push_back(0);
    row.push_back(1);
    row.push_back(2);
    row.push_back(8);

    matrix.push_back(row);
    row.clear();
    row.push_back(0);
    row.push_back(1);
    row.push_back(0);
    row.push_back(4);
    row.push_back(0);
    row.push_back(16);
    matrix.push_back(row);
    row.clear();
    row.push_back(0);
    row.push_back(0);
    row.push_back(1);
    row.push_back(0);
    row.push_back(4);
    row.push_back(12);
    matrix.push_back(row);
    row.clear();

    vector<double> c;
    c.push_back(0);
    c.push_back(0);
    c.push_back(0);
    c.push_back(2);
    c.push_back(3);*/

    LinearProgrammingResult result=linearPSimplexM(matrix,c);
    result.print();
}
