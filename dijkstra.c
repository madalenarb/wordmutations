#include "dijkstra.h"

/******************************************************************************
 * dijkstra()
 *
 * Arguments: fp - pointer to the output
 *            graph
 * 			  main_array
 * 			  src - source word
 *            end - end words
 *            max - maximum changes
 * 			  
 * Returns: void
 *
 * Description: find the shortest dijkstra
 *
 *****************************************************************************/
void dijkstra(FILE* fp, Graph* graph, char** main_array, int src,int end, int max)
{
    //V: extract number of vertices from graph
	int V = graph->num_V;

    //cost: int vector that saves the
    //cost from the source to each vertex
	int cost[V];

    //prev: int vector that saves the previous vectors
	int prev[V];

    //initialize prev
	for(int v = 0; v<V; v++)
		prev[v] = NO_PREV;

    //create heap
	Heap* h = createHeap(V);

    //initialize heap nodes
	for (int v = 0; v < V; v++)
	{
		cost[v] = INT_MAX;
		h->array[v] = newHeapNode(v,cost[v]);
		h->pos[v] = v;
	}
    //update source node cost
	h->array[src]->cost = 0;
	h->pos[src] = src;
	cost[src] = 0;
	decreaseKey(h, src, cost[src]);
	h->size = V;

    //while h contains all nodes whose shortest
    //distance hasn't been finalized
	while (!isEmpty(h))
	{
        //extracts the vertex with mininum
        //distance value
		HeapNode* heapNode =extractMinimum(h);

        //stores the extracted vertex index
		int u = heapNode->v;

        //OPTIMIZATION: Early termination - if we reached the target, stop
		if(u == end) {
			free(heapNode);
			break;
		}

        //goes through all the adjacent vertices of u
        //and updates the cost
		AdjNode* pCrawl =graph->adj_list[u];
		while (pCrawl != NULL)
		{
			int v = pCrawl->vertex;

            //if the distance to the vertex v is not initialized
            //and the cost to v through u is less than the previous one
			if (isInHeap(h, v) && cost[u] != INT_MAX && pCrawl->cost + cost[u] < cost[v] && pCrawl->cost <= max*max )
			{
				cost[v] = cost[u] + pCrawl->cost;
				prev[v]=u;
				// update distance
				decreaseKey(h, v, cost[v]);
			}
			pCrawl = pCrawl->next;
		}
		//free the extracted node
		free(heapNode);
	}
    //print solution
	printSolution(fp, src, end, cost, prev,V, main_array);

    //free remaining heap nodes that weren't extracted (shouldn't be any if algorithm completed)
	for (int v = 0; v < V; v++)
	{
		if (v < h->size && h->array[v] != NULL)
			free(h->array[v]);
	}
	free(h->pos);
	free(h->array);
	free(h);
}
/******************************************************************************
 * heuristic()
 *
 * Arguments: current - current word index
 *            end - end word index
 *            main_array - array of words
 *            word_len - length of words
 * 
 * Returns: estimated cost to reach end from current
 *
 * Description: A* heuristic - minimum cost based on character differences
 *
 *****************************************************************************/
int heuristic(int current, int end, char** main_array, int word_len)
{
    int diff = 0;
    for(int i = 0; i < word_len; i++) {
        if(main_array[current][i] != main_array[end][i])
            diff++;
    }
    return diff * diff; // squared cost
}

/******************************************************************************
 * dijkstra_astar()
 *
 * Arguments: fp - pointer to the output
 *            graph
 *            main_array
 *            src - source word
 *            end - end words
 *            max - maximum changes
 *            word_len - length of words
 * 
 * Returns: void
 *
 * Description: A* variant of Dijkstra with heuristic for better performance
 *
 *****************************************************************************/
void dijkstra_astar(FILE* fp, Graph* graph, char** main_array, int src,int end, int max, int word_len)
{
    int V = graph->num_V;
    int cost[V];
    int prev[V];

    for(int v = 0; v<V; v++)
        prev[v] = NO_PREV;

    Heap* h = createHeap(V);

    for (int v = 0; v < V; v++)
    {
        cost[v] = INT_MAX;
        h->array[v] = newHeapNode(v, INT_MAX);
        h->pos[v] = v;
    }
    
    // Initialize with heuristic
    int h_cost = heuristic(src, end, main_array, word_len);
    h->array[src]->cost = h_cost;
    h->pos[src] = src;
    cost[src] = 0;
    decreaseKey(h, src, h_cost);
    h->size = V;

    while (!isEmpty(h))
    {
        HeapNode* heapNode = extractMinimum(h);
        int u = heapNode->v;

        // OPTIMIZATION: Early termination
        if(u == end) {
            free(heapNode);
            break;
        }

        AdjNode* pCrawl = graph->adj_list[u];
        while (pCrawl != NULL)
        {
            int v = pCrawl->vertex;

            if (isInHeap(h, v) && cost[u] != INT_MAX && pCrawl->cost + cost[u] < cost[v] && pCrawl->cost <= max*max )
            {
                cost[v] = cost[u] + pCrawl->cost;
                prev[v] = u;
                // Use heuristic for priority
                int f_cost = cost[v] + heuristic(v, end, main_array, word_len);
                decreaseKey(h, v, f_cost);
            }
            pCrawl = pCrawl->next;
        }
        free(heapNode);
    }
    
    printSolution(fp, src, end, cost, prev, V, main_array);

    for (int v = 0; v < V; v++)
    {
        if (v < h->size && h->array[v] != NULL)
            free(h->array[v]);
    }
    free(h->pos);
    free(h->array);
    free(h);
}

/******************************************************************************
 * dijkstra_lazy()
 *
 * Arguments: fp - pointer to the output
 *            main_array - array of words
 *            num_words - number of words
 *            word_len - length of words
 *            src - source word index
 *            end - end word index
 *            max - maximum changes
 * 
 * Returns: void
 *
 * Description: Lazy Dijkstra - builds graph on-demand during search
 *              More memory efficient, avoids building unused edges
 *
 *****************************************************************************/
void dijkstra_lazy(FILE* fp, char** main_array, int num_words, int word_len, int src, int end, int max)
{
    int V = num_words;
    int cost[V];
    int prev[V];
    bool visited[V];

    for(int v = 0; v < V; v++) {
        prev[v] = NO_PREV;
        visited[v] = false;
    }

    Heap* h = createHeap(V);

    for (int v = 0; v < V; v++)
    {
        cost[v] = INT_MAX;
        h->array[v] = newHeapNode(v, cost[v]);
        h->pos[v] = v;
    }
    
    h->array[src]->cost = 0;
    h->pos[src] = src;
    cost[src] = 0;
    decreaseKey(h, src, cost[src]);
    h->size = V;

    while (!isEmpty(h))
    {
        HeapNode* heapNode = extractMinimum(h);
        int u = heapNode->v;
        visited[u] = true;

        // OPTIMIZATION: Early termination
        if(u == end) {
            free(heapNode);
            break;
        }

        // LAZY: Generate neighbors on-the-fly instead of pre-built graph
        for(int v = 0; v < V; v++)
        {
            if(visited[v] || v == u)
                continue;

            // Compute edge on-demand
            int diff = diff_letters(main_array[u], main_array[v], word_len, max);
            
            if(diff > 0 && diff <= max) {
                int edge_cost = diff * diff;
                
                if (isInHeap(h, v) && cost[u] != INT_MAX && edge_cost + cost[u] < cost[v])
                {
                    cost[v] = cost[u] + edge_cost;
                    prev[v] = u;
                    decreaseKey(h, v, cost[v]);
                }
            }
        }
        free(heapNode);
    }
    
    printSolution(fp, src, end, cost, prev, V, main_array);

    for (int v = 0; v < V; v++)
    {
        if (v < h->size && h->array[v] != NULL)
            free(h->array[v]);
    }
    free(h->pos);
    free(h->array);
    free(h);
}