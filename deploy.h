#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <vector>
#include "lib_io.h"
#include "lpkit.h"
#define MIN_VALUE 0.01 //用来界定选择服务器
#define MIN_VALUE_ZERO 0.005  //用来界定所有备选服务器
#define MAX_VALUE_SUPER_OUT 2000 //用来设定超源节点与服务器位置关系，作为服务器最大输出系数
#define SERVER_VAL_PARA 1 //服务器价格参数，用来变换服务器价格

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);
bool solveLp();
void getServeLocation(lprec *lp);
void printVector(std::vector<int> v);
	

#endif
