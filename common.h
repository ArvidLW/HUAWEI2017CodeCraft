
/**统一头文件：每个文件只包含这一个文件即可
 *
 * 统一在这:宏定义,库包含,头文件包含
 *
 * 注意：包含顺序，否则可能出错
 *
 * */
#ifndef CDN_COMMON_H
#define CDN_COMMON_H


#define MAXNODE 1000
#define MAXCONSUMER 500
#define MAXVALUE 1000.0
//#define WEDEBUG //用于打印基本信息与计算过程


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "Graph.h"
#include "lwlp.h"
#include "chooseServer.h"




#endif //CDN_COMMON_H
