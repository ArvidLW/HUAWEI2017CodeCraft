#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <vector>
#include "lib_io.h"
#include "lpkit.h"
#define MIN_VALUE 0.01 //用来界定选择服务器
#define MIN_VALUE_ZERO 0.005  //用来界定所有备选服务器

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);
bool solveLp();
void getServeLocation(lprec *lp);
void printVector(std::vector<int> v);
	

#endif
