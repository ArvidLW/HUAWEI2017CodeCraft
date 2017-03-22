#ifndef __ROUTE_H__
#define __ROUTE_H__

#include <vector>
#include "lib_io.h"
#include "lpkit.h"
#define MIN_VALUE 0.01

void deploy_server(char * graph[MAX_EDGE_NUM], int edge_num, char * filename);
bool solveLp();
void getServeLocation(lprec *lp);
void printVector(std::vector<int> v);
	

#endif
