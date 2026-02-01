# Optimizations

Performance improvements while maintaining Dijkstra's algorithm as the core pathfinding method.

## Implementation Details

### 1. Early Termination Fix
File: [dijkstra.c](dijkstra.c)

Corrected the early termination logic to stop when target vertex is reached.

Before:
```c
while (pCrawl != NULL) {
    int v = pCrawl->vertex;
    if(u == end)  // checked inside adjacency loop
        break;
}
```

After:
```c
int u = heapNode->v;
if(u == end) {
    free(heapNode);
    break;
}
```

Stops exploration as soon as target is found.

### 2. A* Variant
File: [dijkstra.c](dijkstra.c)  
Function: `dijkstra_astar()`

A* implementation using an admissible heuristic.

Heuristic function:
```c
int heuristic(int current, int end, char** main_array, int word_len)
{
    int diff = 0;
    for(int i = 0; i < word_len; i++) {
        if(main_array[current][i] != main_array[end][i])
            diff++;
    }
    return diff * diff;
}
```

Uses character difference count as minimum remaining cost estimate. Priority queue ordered by: actual_cost + heuristic_cost.

### 3. Lazy Graph Construction
File: [dijkstra.c](dijkstra.c)  
Function: `dijkstra_lazy()`

Computes edges on-demand during search.

Standard approach:
- Pre-build: O(n² × word_length) comparisons
- Storage: O(n² × edge_density) memory

Lazy approach:
- Build as needed: Only compute edges for explored vertices
- Storage: O(n) memory

Implementation:
Implementation:
```c
for(int v = 0; v < V; v++) {
    if(visited[v] || v == u)
        continue;
    
    int diff = diff_letters(main_array[u], main_array[v], word_len, max);
    if(diff > 0 && diff <= max) {
        int edge_cost = diff * diff;
        // use edge immediately
    }
}
```

Tradeoffs: saves memory for large dictionaries, faster for sparse queries, may be slower for dense query sets due to repeated edge computations.

### 4. Query-Specific Graph Building
File: [graph.c](graph.c)  
Function: `fill_graph_optimized()`

Alternative graph construction that only includes vertices within `max` distance from source or target. Currently available but not used by default modes.

## Mode Configuration

File: [main.c](main.c)

Set `OPTIMIZATION_MODE` to select algorithm:

```c
#define OPTIMIZATION_MODE 3

// Mode 0: Standard Dijkstra with early termination
// Mode 1: A* with heuristic
// Mode 2: Lazy graph construction
// Mode 3: Auto-select based on graph characteristics (RECOMMENDED)
```

### Mode 3: Auto-Selection Logic

Auto mode intelligently chooses the best algorithm for each word size based on multiple factors:

**Factors analyzed:**
- Dictionary size (affects graph build cost)
- Query frequency (affects reuse benefit)
- Graph density (estimated from dict size × word length)
- Build vs search cost tradeoff

**Decision criteria:**

| Condition | Selected | Reason |
|-----------|----------|--------|
| Very large dict (>8000) + sparse queries (<3) | Lazy | Memory critical, avoid O(n²) graph build |
| Small dict (<300) + many queries (≥5) | Dijkstra | Graph reuse amortizes build, simple/fast |
| Very small dict (<200) + single query | Lazy | Skip expensive pre-build |
| Medium dict (500-5000) + many queries (≥3) | A* | Best balance of search and build costs |
| Large dict (>5000) + sparse queries (<3) | Lazy | Memory savings outweigh search cost |
| All other cases | A* | Good default for varied workloads |

The system analyzes the query workload on the first pass and selects optimal modes per word size.

Usage recommendations:

| Scenario | Mode | Notes |
|----------|------|-------|
| Default | 3 | Automatically adapts to workload |
| Large dictionaries | 2 | Avoids memory overhead of graph storage |
| Many queries | 1 | Graph reuse amortizes build cost |
| Few queries | 2 | Skips upfront graph construction |
| Testing/debugging | 0 | Standard Dijkstra baseline |

## Complexity Analysis

| Mode | Graph Build | Per Query | Memory |
|------|-------------|-----------|--------|
| 0 - Dijkstra | O(n² × L) | O((E + V) log V) | O(n²) |
| 1 - A* | O(n² × L) | O(E log V) amortized | O(n²) |
| 2 - Lazy | O(1) | O(n × L × V log V) | O(n) |
| 3 - Auto | Adaptive | Adaptive | Adaptive |

n = words in size group, L = word length, V = vertices = n, E = edges

Mode 3 adapts per word size, using A* where beneficial and Lazy where memory-constrained.

## Testing

Modify `OPTIMIZATION_MODE` in [main.c](main.c) and recompile:

```bash
make clean
make
./wrdmttns testing/sample.dict testing/sample.pals
```
