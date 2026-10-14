/*
    Find the shortest paths from a single source vertex to all other vertices in a weighted graph using the Bellman-Ford algorithm, 
    which works even with negative edge weights. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Edge {
    int source;
    int destination;
    int weight;
}; 

struct Graph { 
    int V; 
    int E; 
    struct Edge* edge;
}; 

struct Graph* createGraph(int V, int E) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph)); 
    graph->V = V; 
    graph->E = E; 
    graph->edge = (struct Edge*)malloc(graph->E * sizeof(struct Edge));
    
    return graph;
} 

void printSolution(int dist[], int n) { 
    printf("Vertex\t Distance from source\n"); 
    for (int i = 0; i < n; ++i) {
        if (dist[i] == INT_MAX) {
            printf("%d \t\t INF\n", i);
        } else { 
            printf("%d \t\t %d\n", i, dist[i]);
        }
    }
} 

void bellmanFord(struct Graph* graph, int source) { 
    int V = graph->V;     
    int E = graph->E;    
    int dist[V];     
    
    for (int i = 0; i < V; i++) { 
        dist[i] = INT_MAX;
    }     
    
    dist[source] = 0; 
    
    for (int i = 1; i <= V - 1; i++) { 
        for (int j = 0; j < E; j++) { 
            int u = graph->edge[j].source;
            int v = graph->edge[j].destination;
            int weight = graph->edge[j].weight;
            
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) { 
                dist[v] = dist[u] + weight; 
            }
        }
    }
    
    for (int j = 0; j < E; j++) { 
        int u = graph->edge[j].source; 
        int v = graph->edge[j].destination;
        int weight = graph->edge[j].weight;
        
        if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) { 
            printf("Graph contains a negative-weight cycle!\n"); 
            return;
        }     
    }     
    
    printSolution(dist, V); 
} 

int main() {     
    int V = 5;     
    int E = 8;     
    struct Graph* graph = createGraph(V, E);     
    
    graph->edge[0].source = 0;     
    graph->edge[0].destination = 1;     
    graph->edge[0].weight = -1;     
    graph->edge[1].source = 0;     
    graph->edge[1].destination = 2;     
    graph->edge[1].weight = 4;     
    graph->edge[2].source = 1;     
    graph->edge[2].destination = 2;     
    graph->edge[2].weight = 3;     
    graph->edge[3].source = 1;     
    graph->edge[3].destination = 3;     
    graph->edge[3].weight = 2;     
    graph->edge[4].source = 1;     
    graph->edge[4].destination = 4;     
    graph->edge[4].weight = 2;     
    graph->edge[5].source = 3; 
    graph->edge[5].destination = 2; 
    graph->edge[5].weight = 5; 
    graph->edge[6].source = 3; 
    graph->edge[6].destination = 1; 
    graph->edge[6].weight = 1; 
    graph->edge[7].source = 4; 
    graph->edge[7].destination = 3; 
    graph->edge[7].weight = -3; 
    
    bellmanFord(graph, 0); 
    
    free(graph->edge); 
    free(graph); 
    
    return 0; 
}
