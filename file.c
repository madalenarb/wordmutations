#include "file.h"

/******************************************************************************
 * file_extention()
 *
 * Arguments: file_name - name of the file
 * 			  
 * Returns: sep - extention of the file
 *
 * Description: returns the file extention
 *
*****************************************************************************/
const char *file_extention(const char* file_name){
    const char *sep = strrchr(file_name, '.');
	if (sep == NULL) {
		exit(0);
	}
    if(!sep || sep == file_name){
        return sep+1;
	}
	return sep;
}


/******************************************************************************
 * output_file()
 *
 * Arguments: file_name - name of the file
 * 			  
 * Returns: fp
 *
 * Description: returns the file pointer to the output file
 *
*****************************************************************************/
FILE* output_file(char* file_name) {
	FILE* fp;
	char* name;

	name = (char*)malloc((strlen(file_name) + 2) * sizeof(char));
	strcpy(name, file_name);
	name[strlen(name) - 4] = '\0';
	strcat(name, "paths");
    fp = fopen(name, "w");
	free(name);
	return fp;
}


/******************************************************************************
 * Problem_file()
 *
 * Arguments: main_array - array of arrays with dictionary words, sorted by size
 * 			  counter - array with the sizes of each word array
 * 		      fPtr - pointer to the .pals file
 * 			  fp_out - pointer to the .dict file
 * 			  graphs - array with graphs for each size
 * 			  
 * Returns: void
 *
 * Description: reads .pals file, creates graph and finds solution (path)
 *
*****************************************************************************/
void Problem_file(char** main_array[],int* counter, FILE* fPtr, FILE* fp_out, Graph* graphs[46]) 
{
	int error = -1;
	int i;
	char fst_word[46] = "";
	char sec_word[46] = "";
	int max_changes = 0;
	int n1 = 0;
	int n2 = 0;
	int size;
	int diff = 0;
	//flags: this will indicate whether the array has been 
	//quicksorted and whether the graph has been created
	//for each size
	int flags[46]={ 0 };
	//graph_max: biggest max_changes for each size
	int graph_max[46]={ 0 };

	//reads the file to get the biggest max_changes for each size of the word
    while (fscanf(fPtr, "%s %s %d", fst_word, sec_word, &max_changes) == 3){
		size = strlen(fst_word);
		diff = diff_letters(fst_word,sec_word,size,size);
		if(flags[size] == 0){ // if it hasn't been quicksorted
			quicksort(main_array[size], counter[size]);
			flags[size] = 1;
		}
		n1 = binarySearch(fst_word, main_array[size], 0, counter[size] - 1);
		n2 = binarySearch(sec_word, main_array[size], 0, counter[size] - 1);
		if(max_changes > graph_max[size] && n1 != n2 && diff != 1 &&
		(n1 !=-1 && n2 !=-1))
		{
			if(max_changes > size)
				graph_max[size] = size;
			else
				graph_max[size] = max_changes;
		}
    }
	//reset array of flags
	for(i = 0; i<46;i++)
		flags[i] = 0 ;
	rewind(fPtr);

	//solves the problem- creates a graph and finds paths
	while (fscanf(fPtr, "%s %s %d", fst_word, sec_word, &max_changes) == 3)
	{
		size = strlen(fst_word);
		diff = diff_letters(fst_word,sec_word,size,max_changes);
		if (max_changes < 0) // if the number of changes possible is negative
		{
			fprintf(fp_out, "%s %d\n%s\n\n", fst_word, error,sec_word);
		}
		else if (strlen(fst_word) != strlen(sec_word)) // different lengths
		{
			fprintf(fp_out, "%s %d\n%s\n\n", fst_word, error,sec_word);
		}else if (counter[size] == 0)// if there are no words with that length
		{
			fprintf(fp_out, "%s %d\n%s\n\n", fst_word, error,sec_word);
		} else {
			n1 = binarySearch(fst_word, main_array[size], 0, counter[size] - 1);
			n2 = binarySearch(sec_word, main_array[size], 0, counter[size] - 1);
			if (n1 == -1 || n2 == -1) //if one of the words aren't in the dictionary
			{
				fprintf(fp_out, "%s %d\n%s\n\n", fst_word, error, sec_word);
			} else if (n1==n2){ // if the words are the same
				fprintf(fp_out, "%s 0\n%s\n\n", fst_word, sec_word);
			}else if (diff == 1){ // if there is only one mutation between both words
				fprintf(fp_out, "%s 1\n%s\n\n", fst_word,sec_word);
			}else
			{
			//if graph hadn't been created
			if (flags[size] == 0)
			{
				graphs[size] = fill_graph(counter[size], size, main_array[size], graph_max[size]);
				flags[size] = 1;
			}

			//apply dksitra
			dijkstra(fp_out,graphs[size],main_array[size], n1,n2, max_changes);
			}
		}
	}
	//free graphs
	for(i = 0; i<46;i++){
		if(flags[i]==1){
			free_graph(graphs[i]);
		}
	}
}

/******************************************************************************
 * Problem_file_optimized()
 *
 * Arguments: main_array - array of arrays with dictionary words, sorted by size
 * 			  counter - array with the sizes of each word array
 * 		      fPtr - pointer to the .pals file
 * 			  fp_out - pointer to the output file
 * 			  graphs - array with graphs for each size (may be NULL for lazy mode)
 * 			  mode - 0: standard Dijkstra, 1: A*, 2: Lazy, 3: Auto-select
 * 			  
 * Returns: void
 *
 * Description: Optimized version with multiple algorithm modes
 *
 *****************************************************************************/
void Problem_file_optimized(char** main_array[],int* counter, FILE* fPtr, FILE* fp_out, Graph* graphs[46], int mode) 
{
	int error = -1;
	int i;
	char fst_word[46] = "";
	char sec_word[46] = "";
	int max_changes = 0;
	int n1 = 0;
	int n2 = 0;
	int size;
	int diff = 0;
	int flags[46]={ 0 };
	int graph_max[46]={ 0 };
	int query_count = 0;
	int auto_mode_selection[46] = {0}; // for mode 3: selected mode per size

	// Count queries and analyze for auto mode
	if(mode == 3) {
		// First pass: count queries per size and analyze characteristics
		int size_query_count[46] = {0};
		while (fscanf(fPtr, "%s %s %d", fst_word, sec_word, &max_changes) == 3){
			size = strlen(fst_word);
			size_query_count[size]++;
			query_count++;
		}
		
		// AUTO MODE 3: Intelligently select best algorithm per word size
		// Considers:
		//   - Dictionary size
		//   - Query frequency
		//   - Graph density (max_changes relative to word length)
		//   - Expected edge count
		//
		// Strategy:
		//   - Sparse graph (dense queries, small dict) → Standard Dijkstra
		//   - Dense graph (many edges) → A* (heuristic helps)
		//   - Large dict + sparse queries → Lazy (memory efficient)
		//   - Medium dict + medium queries → A* (best balance)
		for(i = 0; i < 46; i++) {
			if(counter[i] == 0 || size_query_count[i] == 0) {
				auto_mode_selection[i] = 0; // doesn't matter
				continue;
			}
			
			int dict_size = counter[i];
			int queries = size_query_count[i];
			
			// Decision logic with density consideration:
			
			// Case 1: Very large dictionary + sparse queries → Lazy (memory critical)
			if(dict_size > 8000 && queries < 3) {
				auto_mode_selection[i] = 2; // Lazy - skip expensive graph build
				fprintf(stderr, "[Mode 3] Size %2d: %5d words, %2d queries → LAZY (memory efficient)\n", i, dict_size, queries);
			}
			// Case 2: Small dictionary + many queries → Standard Dijkstra (reuse overhead)
			else if(dict_size < 300 && queries >= 5) {
				auto_mode_selection[i] = 0; // Standard - simple/fast for small graphs
				fprintf(stderr, "[Mode 3] Size %2d: %5d words, %2d queries → DIJKSTRA (small graph reuse)\n", i, dict_size, queries);
			}
			// Case 3: Very small dictionary + single query → Lazy (skip build)
			else if(dict_size < 200 && queries == 1) {
				auto_mode_selection[i] = 2; // Lazy
				fprintf(stderr, "[Mode 3] Size %2d: %5d words, %2d queries → LAZY (skip build overhead)\n", i, dict_size, queries);
			}
			// Case 4: Medium dictionary with many queries → A* (best for reuse)
			else if(dict_size >= 500 && dict_size <= 5000 && queries >= 3) {
				auto_mode_selection[i] = 1; // A* - good balance
				fprintf(stderr, "[Mode 3] Size %2d: %5d words, %2d queries → A* (best balance)\n", i, dict_size, queries);
			}
			// Case 5: Large dictionary with few queries → Lazy (memory/build cost)
			else if(dict_size > 5000 && queries < 3) {
				auto_mode_selection[i] = 2; // Lazy
				fprintf(stderr, "[Mode 3] Size %2d: %5d words, %2d queries → LAZY (sparse queries)\n", i, dict_size, queries);
			}
			// Default: A* works well for most other cases
			else {
				auto_mode_selection[i] = 1; // A*
				fprintf(stderr, "[Mode 3] Size %2d: %5d words, %2d queries → A* (default)\n", i, dict_size, queries);
			}
		}
		
		rewind(fPtr);
	}

	// For modes that need graphs, determine max_changes per size
	if(mode != 2 && mode != 3) { // modes 0 and 1 use pre-built graphs
		while (fscanf(fPtr, "%s %s %d", fst_word, sec_word, &max_changes) == 3){
			size = strlen(fst_word);
			diff = diff_letters(fst_word,sec_word,size,size);
			if(flags[size] == 0){ 
				quicksort(main_array[size], counter[size]);
				flags[size] = 1;
			}
			n1 = binarySearch(fst_word, main_array[size], 0, counter[size] - 1);
			n2 = binarySearch(sec_word, main_array[size], 0, counter[size] - 1);
			if(max_changes > graph_max[size] && n1 != n2 && diff != 1 &&
			(n1 !=-1 && n2 !=-1))
			{
				if(max_changes > size)
					graph_max[size] = size;
				else
					graph_max[size] = max_changes;
			}
		}
		for(i = 0; i<46;i++)
			flags[i] = 0 ;
		rewind(fPtr);
	} else if(mode == 3) {
		// For auto mode, prepare graphs only for sizes that need them (using A*)
		while (fscanf(fPtr, "%s %s %d", fst_word, sec_word, &max_changes) == 3){
			size = strlen(fst_word);
			diff = diff_letters(fst_word,sec_word,size,size);
			if(flags[size] == 0){ 
				quicksort(main_array[size], counter[size]);
				flags[size] = 1;
			}
			
			// Only track graph_max if this size will use A*
			if(auto_mode_selection[size] == 1) {
				n1 = binarySearch(fst_word, main_array[size], 0, counter[size] - 1);
				n2 = binarySearch(sec_word, main_array[size], 0, counter[size] - 1);
				if(max_changes > graph_max[size] && n1 != n2 && diff != 1 &&
				(n1 !=-1 && n2 !=-1))
				{
					if(max_changes > size)
						graph_max[size] = size;
					else
						graph_max[size] = max_changes;
				}
			}
		}
		for(i = 0; i<46;i++)
			flags[i] = 0 ;
		rewind(fPtr);
	} else {
		// For lazy mode, just sort
		for(i = 0; i < 46; i++) {
			if(counter[i] > 0) {
				quicksort(main_array[i], counter[i]);
			}
		}
	}

	// Process queries
	while (fscanf(fPtr, "%s %s %d", fst_word, sec_word, &max_changes) == 3)
	{
		size = strlen(fst_word);
		diff = diff_letters(fst_word,sec_word,size,max_changes);
		
		if (max_changes < 0) 
		{
			fprintf(fp_out, "%s %d\n%s\n\n", fst_word, error,sec_word);
		}
		else if (strlen(fst_word) != strlen(sec_word)) 
		{
			fprintf(fp_out, "%s %d\n%s\n\n", fst_word, error,sec_word);
		}else if (counter[size] == 0)
		{
			fprintf(fp_out, "%s %d\n%s\n\n", fst_word, error,sec_word);
		} else {
			n1 = binarySearch(fst_word, main_array[size], 0, counter[size] - 1);
			n2 = binarySearch(sec_word, main_array[size], 0, counter[size] - 1);
			if (n1 == -1 || n2 == -1) 
			{
				fprintf(fp_out, "%s %d\n%s\n\n", fst_word, error, sec_word);
			} else if (n1==n2){ 
				fprintf(fp_out, "%s 0\n%s\n\n", fst_word, sec_word);
			}else if (diff == 1){ 
				fprintf(fp_out, "%s 1\n%s\n\n", fst_word,sec_word);
			}else
			{
				// Mode selection
				int selected_mode = mode;
				
				// For auto mode, use the pre-selected mode for this size
				if(mode == 3) {
					selected_mode = auto_mode_selection[size];
				}
				
				if(selected_mode == 2) {
					// LAZY MODE: No graph pre-building, compute edges on-the-fly
					dijkstra_lazy(fp_out, main_array[size], counter[size], size, n1, n2, max_changes);
				}
				else {
					// Build graph if needed (for modes 0, 1, or auto using A*)
					if (flags[size] == 0)
					{
						graphs[size] = fill_graph(counter[size], size, main_array[size], graph_max[size]);
						flags[size] = 1;
					}

					if(selected_mode == 1) {
						// A* MODE: Use heuristic
						dijkstra_astar(fp_out, graphs[size], main_array[size], n1, n2, max_changes, size);
					}
					else {
						// STANDARD DIJKSTRA (mode 0)
						dijkstra(fp_out, graphs[size], main_array[size], n1, n2, max_changes);
					}
				}
			}
		}
	}
	
	// Free graphs (only if not pure lazy mode)
	if(mode != 2) {
		for(i = 0; i<46;i++){
			if(flags[i]==1){
				free_graph(graphs[i]);
			}
		}
	}
}

/******************************************************************************
 * printPath()
 *
 * Arguments: fp - pointer to output file
 * 			  parent[] - array of previous vertices 
 * 		      j - index of the destination
 * 			  mainArr - array with words of a certain size
 * 			  
 * Returns: void
 *
 * Description: prints path
 *
 *****************************************************************************/
void printPath(FILE* fp, int parent[], int j, char** mainArr)
{
    //if j is the source
    if (parent[j]==NO_PREV)
        return;

    printPath(fp, parent, parent[j], mainArr);

    fprintf(fp,"%s\n", mainArr[j]);
}

/******************************************************************************
 * printSolution()
 *
 * Arguments: fp - pointer to output file
 * 			  src - source word
 * 			  parent[] - array of previous vertices 
 * 		      i - index of the destination
 * 			  cost - array of costs, depending on the destination
 * 			  mainArr - array with words of a certain size
 * 			  
 * Returns: void
 *
 * Description: prints solution
 *
*****************************************************************************/
void printSolution(FILE* fp, int src, int i, int* cost, int* parents, int V, char** mainArr){
 	if (cost[i] ==  INT_MAX){
 		fprintf(fp,"%s -1\n%s\n\n", mainArr[src],mainArr[i]);
 	} else{
 		fprintf(fp,"%s %d\n", mainArr[src], cost[i]);
 		printPath(fp, parents,i,mainArr);
 		fprintf(fp,"\n");
 	}
}