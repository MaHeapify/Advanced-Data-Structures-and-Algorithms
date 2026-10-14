/*
    Aim: Implement Johnson's algorithm to find the shortest paths between all pairs of vertices in a sparse directed graph with positive and negative edge weights (no negative cycles). 
    
    Problem Statement: You are given a directed, weighted graph with V vertices and E edges. Some edges may have negative weights, but the graph does not contain negative weight cycles. 
    
    Your task is to: 
    1. Read the graph from input. 
    2. Implement Johnson’s Algorithm to compute the shortest path between all pairs of vertices. 
    3. Display the shortest path distance matrix. 
    
    Input Format: 
        First line: Two integers V and E (number of vertices and edges).
        Next E lines: Three integers u, v, w each, denoting an edge from vertex u to vertex v with weight w. 
        
    Output Format: 
        A V x V matrix where the entry at row i and column j represents the shortest distance from vertex i to vertex j. 
        If there is no path between two vertices, print INF. 
        
    Constraints: 
        1 ≤ V ≤ 1000 
        1 ≤ E ≤ 5000 
        -1000 ≤ w ≤ 1000
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define INF LLONG_MAX

typedef long long ll;

typedef struct { 
    int u, v; 
    ll w; 
} Edge; 

typedef struct { 
    int v; 
    ll w; 
    int next; 
} AdjEdge; 

typedef struct { 
    ll dist; 
    int vertex; 
} Node; 

// ---------- Dynamic Min Heap ---------- 
typedef struct { 
    Node *arr; 
    int size; 
    int capacity; 
} MinHeap; 

void swap(Node *a, Node *b) { 
    Node temp = *a; 
    *a = *b; 
    *b = temp; 
} 

MinHeap* createHeap(int capacity) {
    MinHeap *heap = (MinHeap*)malloc(sizeof(MinHeap));     
    
    if (!heap) {
        return NULL; 
    } 
    heap->arr = (Node*)malloc(capacity * sizeof(Node));     
    heap->size = 0;     
    heap->capacity = capacity;     
    
    return heap; 
} 

void push(MinHeap *heap, Node node) {     
    if (heap->size >= heap->capacity) {         
        heap->capacity *= 2;         
        Node *temp = (Node*)realloc(heap->arr, heap->capacity * sizeof(Node));         
        
        if (temp) {             
            heap->arr = temp;         
        } else {             
            return;         
        }     
    }     
    
    int i = heap->size++;     
    heap->arr[i] = node;
    
    while (i > 0) { 
        int parent = (i - 1) / 2;         
        
        if (heap->arr[parent].dist <= heap->arr[i].dist) {
            break;
        }         
        
        swap(&heap->arr[parent], &heap->arr[i]);         
        i = parent;     
    } 
} 

Node pop(MinHeap *heap) {     
    Node result = heap->arr[0];     
    heap->arr[0] = heap->arr[--heap->size];     
    int i = 0;     
    
    while (1) {         
        int left = 2 * i + 1;         
        int right = 2 * i + 2;         
        int smallest = i;         
        
        if (left < heap->size && heap->arr[left].dist < heap->arr[smallest].dist) {             
            smallest = left;         
        }         
        
        if (right < heap->size && heap->arr[right].dist < heap->arr[smallest].dist) {             
            smallest = right;
        }         
        
        if (smallest == i) {
            break;         
        }
        
        swap(&heap->arr[i], &heap->arr[smallest]);         
        i = smallest;     
    }     
    
    return result; 
} 

int isEmpty(MinHeap *heap) {     
    return heap->size == 0; 
} 

int main() {     
    int V, E;     
    
    // Interactive prompt to fix the blank screen issue    
    printf("\nEnter number of vertices and edges (e.g., 5 8): ");     
    fflush(stdout);     
    
    if (scanf("%d %d", &V, &E) != 2) {
        return 0;
    }     
    
    // Allocate structures defensively based on edge limits     
    Edge *edges = (Edge*)malloc((E + V) * sizeof(Edge));     
    AdjEdge *adj = (AdjEdge*)malloc(E * sizeof(AdjEdge));     
    int *head = (int*)malloc(V * sizeof(int));     
    ll *h = (ll*)malloc((V + 1) * sizeof(ll));     
    ll **dist = (ll**)malloc(V * sizeof(ll *));     
    
    for (int i = 0; i < V; i++) {         
        head[i] = -1;     
    }     
    
    // Read input graph while detecting and fixing 1-based indexing     
    printf("\nEnter the %d edges in format (source destination weight):\n", E);     
    fflush(stdout);     
    
    int uses_one_indexing = 0;     
    for (int i = 0; i < E; i++) {         
        if (scanf("%d %d %lld", &edges[i].u, &edges[i].v, &edges[i].w) != 3) {
            return 0;
        }         
        
        // If the code sees any vertex equal to V, it assumes 1-based indexing layout         
        if (edges[i].u == V || edges[i].v == V) {             
            uses_one_indexing = 1;         
        }     
    }     
    
    if (uses_one_indexing) {         
        for (int i = 0; i < E; i++) {             
            edges[i].u--;             
            edges[i].v--;         
        }     
    }     
    
    // Step 1: Add a virtual master source vertex S = V
    int S = V;     
    for (int i = 0; i < V; i++) {         
        edges[E + i].u = S;         
        edges[E + i].v = i;         
        edges[E + i].w = 0;     
    }     
    
    int totalEdges = E + V; 
    
    // Step 2: Compute Vertex Potentials via Bellman-Ford     
    for (int i = 0; i <= V; i++) {         
        h[i] = INF;     
    }     
    
    h[S] = 0;     
    for (int i = 0; i < V; i++) {         
        int updated = 0;         
        for (int j = 0; j < totalEdges; j++) {             
            int u = edges[j].u;             
            int v = edges[j].v;             
            ll w = edges[j].w;             
            
            if (h[u] != INF && h[u] + w < h[v]) {                 
                h[v] = h[u] + w;                 
                updated = 1;             
            }         
        }         
        
        if (!updated) {
            break;
        }     
    }     
    
    // Detect structural negative weight cycle anomaly     
    for (int i = 0; i < totalEdges; i++) {         
        int u = edges[i].u;         
        int v = edges[i].v;         
        ll w = edges[i].w;         
        
        if (h[u] != INF && h[u] + w < h[v]) {             
            printf("\nNegative weight cycle detected!\n");             
            free(edges); 
            free(adj); 
            free(head); 
            free(h); 
            free(dist);             
            
            return 0;         
        }     
    }     
    
    // Step 3: Reweight structural edges to be consistently non-negative     
    for (int i = 0; i < E; i++) {         
        int u = edges[i].u;         
        int v = edges[i].v;         
        
        adj[i].v = v;         
        adj[i].w = edges[i].w + h[u] - h[v];         
        adj[i].next = head[u];         
        head[u] = i;     
    }     
    
    // Allocate final workspace matrix boundaries     
    for (int i = 0; i < V; i++) {         
        dist[i] = (ll*)malloc(V * sizeof(ll));         
        
        for (int j = 0; j < V; j++) {             
            dist[i][j] = INF;         
        }     
    }     
    
    // Step 4: Run Dijkstra from every vertex natively     
    for (int src = 0; src < V; src++) {         
        ll *d = (ll*)malloc(V * sizeof(ll));         
        
        for (int i = 0; i < V; i++) {
            d[i] = INF;
        }         
        
        MinHeap *heap = createHeap(E > 0 ? E : 1);         
        
        d[src] = 0;         
        push(heap, (Node){0, src});         
        
        while (!isEmpty(heap)) {             
            Node current = pop(heap);             
            ll du = current.dist;             
            int u = current.vertex;             
            
            if (du != d[u]) {
                continue;
            }             
            
            for (int i = head[u]; i != -1; i = adj[i].next) {                 
                int v = adj[i].v;                 
                ll w = adj[i].w;                 
                
                if (d[u] != INF && d[u] + w < d[v]) {                     
                    d[v] = d[u] + w;                     
                    push(heap, (Node){d[v], v});                 
                }             
            }         
        }         
            
        // Step 5: Convert modified values back to real source distance weightings         
        for (int v = 0; v < V; v++) {             
            if (d[v] != INF) {                 
                dist[src][v] = d[v] - h[src] + h[v];             
            }         
        }         
            
        free(d);         
        free(heap->arr);         
        free(heap);     
    }     
        
    // Step 6: Print accurate path matrix layout metrics     
    printf("\nShortest Path Distance Matrix:\n");     
    for (int i = 0; i < V; i++) {         
        for (int j = 0; j < V; j++) {             
            if (dist[i][j] == INF) {
                printf("INF "); 
            } else {
                printf("%lld ", dist[i][j]);
            }         
        }         
            
        printf("\n");     
    }     
        
    // Free all dynamically allocated memory cleanly     
    for (int i = 0; i < V; i++) {         
        free(dist[i]);     
    }     
        
    free(dist); 
    free(edges); 
    free(adj); 
    free(head); 
    free(h);     
        
    return 0; 
}
