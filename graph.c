#include "graph.h"

/******************************************************************************
 * diff_letters()
 *
 * Arguments: w1 - fist word
 * 			      w2 - second word
 *            wordLength - length of a word
 *            max - maximum changes allowed
 * 			  
 * Returns: count
 *
 * Description: returns the number of different letters between two words
 *
*****************************************************************************/
int diff_letters(char* w1, char* w2, int wordLength,int max)
{
    int count=0, i;
    for (i = 0; i < wordLength; i++) {
        if (w1[i] != w2[i]) {
            count++;
        }
        if (count > max)
            return -1;
    }
    //printf("comparing %s with %s result is %d\n", w1, w2, count);
    return count;
}

/******************************************************************************
 * createNode()
 *
 * Arguments: vertex - destination
 * 			  cost - cost from the source to the vertex
 * 			  
 * Returns: newNode
 *
 * Description: returns a new node
 *
*****************************************************************************/
AdjNode* createNode(int vertex, int cost){
	AdjNode* newNode = (AdjNode*)malloc(sizeof(AdjNode));
	newNode->vertex = vertex;
	newNode->cost = cost;
	newNode->next = NULL;
	return newNode;
}


/******************************************************************************
 * createGraph()
 *
 * Arguments: v - number of vertices
 * 			  
 * Returns: graph
 *
 * Description: returns a new graph
 *
*****************************************************************************/
Graph* createGraph(int v){
	Graph* graph = (Graph*)malloc(sizeof(Graph));
	graph->num_V = v;
	graph->adj_list = (AdjNode**)malloc(v * sizeof(AdjNode*));//array
	for(int i = 0; i<v;i++){
		graph->adj_list[i]=NULL;
	}
	return graph;
}


/******************************************************************************
 * add_edge()
 *
 * Arguments: graph - graph to which the edge will be added
 * 			  src - index of the source word
 * 			  dest - index of the destination word
 * 			  diff- number of different letters between 2 words
 * 			  
 * Returns: void
 *
 * Description: adds edge to graph
 *
*****************************************************************************/
void add_edge(Graph* graph, int src, int dest, int diff){
    //determine de cost between src and dest
    int cost = diff*diff;

    AdjNode* newNode = createNode(dest,cost);
    newNode->next = graph->adj_list[src];
    graph->adj_list[src] = newNode;

    newNode = createNode(src, cost);
    newNode->next = graph->adj_list[dest];
    graph->adj_list[dest] = newNode;

}

/******************************************************************************
 * fill_graph()
 *
 * Arguments: num_of_words - number of words in the array of a certain size
 * 			  size_of_words
 * 			  words_dict - array of arrays of dictionary words, sorted by word length
 * 			  max - maximum changed that can be made
 * 			  
 * Returns: graph
 *
 * Description: fills a graph
 *
*****************************************************************************/
Graph* fill_graph(int num_of_words, int size_of_words, char** words_dict,int max){
    int dest,src;
    Graph* graph = createGraph(num_of_words);
    int diff;
    for(src=0; src < num_of_words; src++)
    {
        for(dest=src+1; dest<num_of_words;dest++)
        {
            diff = diff_letters(words_dict[src], words_dict[dest], size_of_words,max);
            if(diff>0)
                add_edge(graph, src, dest, diff);
        }
    }
    return(graph);
}

/******************************************************************************
 * free_graph()
 *
 * Arguments: graph - graph to be freed
 * 			  
 * Returns: void
 *
 * Description: free graph
 *
*****************************************************************************/
void free_graph(Graph *graph) {
  if (graph != NULL) {
    if (graph->adj_list != NULL) {
      // Free the adjacency list array
      for (int v = 0; v < graph->num_V; v++) {
        AdjNode *item = graph->adj_list[v];

        while (item != NULL) {
          AdjNode *tmp = item->next;
          free(item);
          item = tmp;
        }
      }
    }
    // Free the graph
    free(graph->adj_list);
    free(graph);
  }
}

/******************************************************************************
 * fill_graph_optimized()
 *
 * Arguments: num_of_words - number of words in the array of a certain size
 *            size_of_words
 *            words_dict - array of arrays of dictionary words, sorted by word length
 *            max - maximum changes that can be made
 *            src - source word index
 *            end - target word index
 * 
 * Returns: graph
 *
 * Description: Optimized graph construction - only builds edges relevant to the query
 *              Uses bidirectional growth from src and end to reduce edge count
 *
 *****************************************************************************/
Graph* fill_graph_optimized(int num_of_words, int size_of_words, char** words_dict, int max, int src, int end)
{
    Graph* graph = createGraph(num_of_words);
    int diff;
    
    // Build edges only from vertices that could be on the path
    // This is still an approximation but better than full graph
    for(int i = 0; i < num_of_words; i++)
    {
        // Check distance from source
        int dist_from_src = diff_letters(words_dict[i], words_dict[src], size_of_words, size_of_words);
        int dist_to_end = diff_letters(words_dict[i], words_dict[end], size_of_words, size_of_words);
        
        // Only build edges for words that could plausibly be on a path
        if(dist_from_src >= 0 && dist_to_end >= 0 && (dist_from_src <= max || dist_to_end <= max || i == src || i == end))
        {
            for(int j = i + 1; j < num_of_words; j++)
            {
                int dist_from_src_j = diff_letters(words_dict[j], words_dict[src], size_of_words, size_of_words);
                int dist_to_end_j = diff_letters(words_dict[j], words_dict[end], size_of_words, size_of_words);
                
                if(dist_from_src_j >= 0 && dist_to_end_j >= 0 && 
                   (dist_from_src_j <= max || dist_to_end_j <= max || j == src || j == end))
                {
                    diff = diff_letters(words_dict[i], words_dict[j], size_of_words, max);
                    if(diff > 0)
                        add_edge(graph, i, j, diff);
                }
            }
        }
    }
    return graph;
}
