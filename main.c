/**
 * @file main.c
 * @author João Lopes
 * @author Madalena Barros
 * @brief Word Mutations
 * @version 2
 * @date 2022-10-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/*Import System Files*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

/*Import Local Files*/
#include "array.h"
#include "file.h"
#include "graph.h"
#include "heap.h"

// Optimization mode:
// Mode 0: Standard Dijkstra with early termination
// Mode 1: A* with heuristic
// Mode 2: Lazy graph construction
// Mode 3: Auto-select based on graph characteristics (RECOMMENDED)
#define OPTIMIZATION_MODE 3

/*main*/
int main(int argc, char* argv[])
{
	FILE* fp_dict;
	FILE* fp_pals;
	FILE* fp_out;
	char* file_pals, *file_dict;
	const char* file_pals_ext, * file_dict_ext;
	int selected_mode = OPTIMIZATION_MODE;
	if (argc != 3 && argc != 4) {
	 	exit(0);
	}
	file_dict = argv[1];
	file_dict_ext = file_extention(file_dict);

	if (strcmp(file_dict_ext, ".dict")) {
	 	exit(0);
	}

	file_pals = argv[2];
	file_pals_ext = file_extention(file_pals);

	if (strcmp(file_pals_ext, ".pals")) {
	 	exit(0);
	}

	if ((fp_pals = fopen(file_pals, "r")) == NULL) {
	 	exit(0);
	}

	if ((fp_dict = fopen(file_dict, "r")) == NULL) {
	 	if (fp_pals != NULL) {
	 		fclose(fp_pals);
	 	}
	 	exit(0);
	}

	if (argc == 4) {
		int mode_arg = atoi(argv[3]);
		if (mode_arg >= 0 && mode_arg <= 3) {
			selected_mode = mode_arg;
		}
	}

	//main_array: strings array with words from the dictionary, sorted by size
	char** main_array[47];

	// counter: int array that saves the number of dictionary words of each size
	int counter[47];

	//graphs: graph for each size 
	Graph* graphs[46];
	array(fp_dict,main_array,counter);
	fclose(fp_dict);

	fp_out = output_file(file_pals);

	// Use optimized version with A* (mode 1) for better performance
	// Mode 0 = Standard Dijkstra with early termination
	// Mode 1 = A* with heuristic (recommended)
	// Mode 2 = Lazy graph construction (best for sparse queries)
	Problem_file_optimized(main_array, counter, fp_pals, fp_out, graphs, selected_mode);

	free_all(main_array, counter);
	fclose(fp_pals);
	fclose(fp_out);


	return 0;
	// char wl[10][6] = {"aaaaa","aaaab","aaaac","aaaaf","aaagh","aakjh","aaall","kidaa","lolui","aaagh"};

	// Graph* graph = fill_graph(wl);

	// printGraph(graph);

}
