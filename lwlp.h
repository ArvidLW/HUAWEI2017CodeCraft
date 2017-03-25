//
// Created by lw_co on 2017/3/21.
//

#ifndef CDN_LWLP_H
#define CDN_LWLP_H


#include <iostream>
#include <vector>

struct LinearRe {
    std::vector<double> y_;//单纯形算子
    std::vector<std::vector<double>> B_;//B逆，初始为单位矩阵，中间变换矩阵
    //std::vector<double> numB;//基向量相对位置对应编号
    //std::vector<double> numN;//非基向量对应编号
    std::vector<int> numVar;//基变量与非基变量，对应实际的位置编号，开始时前h个是基变量
    std::vector<double> inVarFactor;//换入变量的系数计算
    std::vector<std::vector<double >> *mc;
    std::vector<double> *co;
    int h,l,s;
    double minAlpha1{0.0};
    double minAlpha{0.0};
    double object{0};
    //小入基大出基,对于min标准型
    int inNum{-1};//换入列变量编号（在非基中的相对编号）
    int outNum{-1};//换出行变量编号（在基中位置，相对编号），可以通过numB也转成列变量编号

    void run();
    void y_Cal();
    bool z_NCal();
    void updateFactor();
    void chooseOut();
    void updateResource();
    void updateB_();
    LinearRe(std::vector<std::vector<double >> *matrixC,std::vector<double> *vectorO){

        mc=matrixC;
        co=vectorO;
        h=(signed)matrixC->size();
        l=(signed)(*matrixC)[0].size();
        s=(signed)vectorO->size();
        numVar.resize(l);
        //最后一列为资源向量
        if(l!=s+1){
            std::cout<<"l="<<l<<",s="<<s<<", input error!"<<std::endl;
            exit(10);
        }
        y_.resize(h);
        B_.resize(h);
        inVarFactor.resize(h);
        for(int i=0;i<h;++i){
            B_[i].resize(h);
            B_[i][i]=1;
        }
        for(int i=0;i< l;++i){
            numVar[i]=i;
        }

    }

};


void LinearRe::run(){
    while(1){
        //算子
        y_Cal();
        //检验数与换出变量
        if(z_NCal()){
            break;
        }
        //更新系数
        updateFactor();
        //换出变量
        chooseOut();
        //更新换出变量系数
        //updateOutFactor();
        //更新资源向量，计算目标值
        updateResource();
        //更新B_
        updateB_();
    }


}

//1、单纯形算子
// B_T:B 逆的转置
//CB:基变量对价值系统
//用B_T的每行乘以CB的每列
//y_=B_T*CB
void LinearRe::y_Cal(){
    for(int j=0;j<h;++j){
        double s=0;
        for(int i=0;i<h;++i){
            s=s+B_[i][j]*(*co)[numVar[i] ];
        }
        y_[j]=s;
    }
}

//2、检验数，选入基变量
// Z_N=CN-NTY_，Z_N为检验数
//C0N表示原始的输入目标向量,其从k之后开始才是CN
//NT是我们需要的，输入N要用其转置，注意标号
//计算检验数，并选出要换入的标号，若检验数都大于等于零则停止（获得最优解）
bool LinearRe::z_NCal(){
    double min=0,checkNum;
    int inNumtmp{-1};
    for(int j=h;j<s;++j){
        double s=0;
        for(int i=0;i<h;++i){
            s=s+(*mc)[i][numVar[j]]*y_[i];
        }
        //选择检验数最小的，记录其位置
        checkNum=(*co)[numVar[j] ]-s;
        if(checkNum<min){
            min=checkNum;
            inNumtmp=j;
        }
    }
    if(inNumtmp==-1){
        std::cout <<"gain the best result!"<<std::endl;
        std::cout <<"object:"<<object<<std::endl;

#ifdef WEDEBUG
        for(int i=0; i<h;++i){
            printf("val_%d =%.2f\n",numVar[i],(*mc)[i][l-1]);
        }
#endif
        return true;
    }
    inNum=inNumtmp;
    minAlpha=min;
    return false;

}
//3、
// 计算变换后的矩阵要换入基的那一列的系数计算
void LinearRe::updateFactor(){
    if(inNum==-1){
        std::cout <<"updateFactor error!"<<std::endl;
        exit(2);
    }
    bool geZeroflag{false};
    std::vector<double> newCol;
    for(int i=0;i<h;++i){
        double s=0;
        for(int j=0;j<h;++j){
            s=s+B_[i][j]* (*mc)[j][numVar[inNum] ];
        }
        if(s>0){geZeroflag=true;}
        newCol.push_back(s);
    }
    if(!geZeroflag){
        std::cout<<"no bound question!"<<std::endl;
        exit(3);
    }
    //将计算得到的值赋给inVarFactor
    for(int i=0;i<h;++i){
        //用于后面计算资源向量
        inVarFactor[i]=newCol[i];
        //更新换入变量系数
        (*mc)[i][numVar[inNum] ]=newCol[i];
    }
}
//4、确定换出基
void LinearRe::chooseOut(){
    double min1{1.0e6};
    double tmp{0};
    int outNumTmp=-1;
    for(int i=0;i<h;++i){
        if(inVarFactor[i]>0){
            tmp=(*mc)[i][l-1]/inVarFactor[i];

            if(tmp<min1){
                min1=tmp;
                outNumTmp=i;
            }
        }
    }
    if(outNumTmp==-1){
        std::cout<<"Find out var error!"<<std::endl;
        exit(20);
    }
    outNum=outNumTmp;
    minAlpha1=min1;
}

//5、更新资源向量,计算目标值，更换基变量位置并记录
void LinearRe::updateResource(){
    for(int i=0;i<h;++i){
        (*mc)[i][l-1]=(*mc)[i][l-1]-minAlpha1*(*mc)[i][numVar[inNum] ];
        // std::cout<<"mc: "<<(*mc)[i][l-1]<<std::endl;
    }
    //(*mc)[numVar[outNum]][l-1]=minAlpha1;
    (*mc)[outNum][l-1]=minAlpha1;
    object=object+minAlpha1*minAlpha;
    int tmp;
    tmp=numVar[outNum];
    numVar[outNum]=numVar[inNum];
    numVar[inNum]=tmp;
}
//6、更新B_,矩阵初等行变换，由换出位置知道哪个是主元，由换进位置知道对哪列进行单位化
void LinearRe::updateB_(){
    double k;
    k=1.0/inVarFactor[outNum];
    if(k!=1){
        for(int i=0;i<h;++i)
        {
            inVarFactor[i]=-k*inVarFactor[i];
        }
    }
    //上面全部转化负的，现在转换过来
    inVarFactor[outNum]=-inVarFactor[outNum];
    //左乘以B_进行行变换
    for(int i=0;i<h;++i){
        if(i!=outNum){
            if(inVarFactor[i]!=0){
                //把invarFactor[i]倍的第outNum行加到第i行
                for(int j=0;j<h;++j){
                    B_[i][j]=B_[i][j]+inVarFactor[i]*B_[outNum][j];
                }
            }
        }
    }
    if(k!=1){
        for(int j=0;j<h;++j){
            B_[outNum][j]=k*B_[outNum][j];
        }

    }
    //清空以免影响后面
    inVarFactor.clear();

}


#endif //CDN_LWLP_H


