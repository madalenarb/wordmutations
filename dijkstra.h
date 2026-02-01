#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

/*Import System Files*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "graph.h"
#include "heap.h"
#include "file.h"

#define NO_PREV -1

void dijkstra(FILE* fp, Graph* graph, char** main_array, int src,int end, int max);
void dijkstra_astar(FILE* fp, Graph* graph, char** main_array, int src,int end, int max, int word_len);
void dijkstra_lazy(FILE* fp, char** main_array, int num_words, int word_len, int src, int end, int max);

#endif