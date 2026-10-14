/*
    Aim: Implement a van Emde Boas (vEB) Tree using recursive structure and demonstrate key operations.
    
    Tasks:
        a) Implement a vEB Tree with universe size U = 16.
        b) Implement and test the following operations:
            Insert(x)
            Member(x)
            Successor(x) and Predecessor(x)
            Delete(x)
        c) Demonstrate the recursive layout of the structure and print results of each operation.
        
    Expected Output:
        Structure visualization (showing clusters and summary).
        Correct output for predecessor/successor queries.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#define NIL -1

typedef struct vEB {
    int u;
    int min, max;
    struct vEB *summary;
    struct vEB **cluster;
} vEB;

// Helper math functions
int high(int x, int u) {
    int lower_sqrt = (int)sqrt(u);
    return x / lower_sqrt;
} 

int low(int x, int u) {
    int lower_sqrt = (int)sqrt(u);
    return x % lower_sqrt;
}

int indexVal(int h, int l, int u) {
    int lower_sqrt = (int)sqrt(u);
    return h * lower_sqrt + l;
}

// Create a vEB tree of universe size u (must be power of 2)
vEB* createvEB(int u) {
    vEB *V = (vEB*)malloc(sizeof(vEB));
    V->u = u;
    V->min = NIL;
    V->max = NIL;
    
    if (u <= 2) {
        V->summary = NULL;
        V->cluster = NULL;
    } else {
        int upper_sqrt = (int)sqrt(u);
        
        V->summary = createvEB(upper_sqrt);
        
        V->cluster = (vEB**)malloc(sizeof(vEB*) * upper_sqrt);
        
        for (int i = 0; i < upper_sqrt; i++) {
            V->cluster[i] = createvEB(upper_sqrt);
        }
    }
    return V;
} 

// Check membership: O(log log u)
bool isMember(vEB *V, int x) {
    if (V == NULL || x >= V->u || x < 0) {
        return false;
    }
    
    if (x == V->min || x == V->max) {
        return true;
    }
    
    if (V->u <= 2) {
        return false;
    } 
    
    return isMember(V->cluster[high(x, V->u)], low(x, V->u));
}

// Find Successor: O(log log u)
int successor(vEB *V, int x) {
    if (V->u <= 2) { 
        if (x == 0 && V->max == 1) {
            return 1; 
        }
            
        return NIL;
    }
    
    if (V->min != NIL && x < V->min) {
        return V->min;
    }
    
    int h = high(x, V->u), l = low(x, V->u);
    int max_in_cluster = V->cluster[h]->max;
    
    if (max_in_cluster != NIL && l < max_in_cluster) {
        int offset = successor(V->cluster[h], l);
        return indexVal(h, offset, V->u);
    } else {
        int succ_cluster = successor(V->summary, h);
        
        if (succ_cluster == NIL) {
            return NIL;
        } 
        
        int offset = V->cluster[succ_cluster]->min;
        
        return indexVal(succ_cluster, offset, V->u);
    }
} 

// Find Predecessor: O(log log u)
int predecessor(vEB *V, int x) {
    if (V->u <= 2) {
        if (x == 1 && V->min == 0) {
            return 0;
        } return NIL;
    } 
    if (V->max != NIL && x > V->max) {
        return V->max;
    }
    
    int h = high(x, V->u), l = low(x, V->u);
    int min_in_cluster = V->cluster[h]->min;
    if (min_in_cluster != NIL && l > min_in_cluster) {
        int offset = predecessor(V->cluster[h], l);
        
        return indexVal(h, offset, V->u);
    } else {
        int pred_cluster = predecessor(V->summary, h);
        
        if (pred_cluster == NIL) {
            if (V->min != NIL && x > V->min) {
                return V->min; return NIL;
            } 
            
            int offset = V->cluster[pred_cluster]->max;
            
            return indexVal(pred_cluster, offset, V->u);
        }
    }
}

// Empty tree insertion
void emptyTreeInsert(vEB *V, int x) {
    V->min = x; V->max = x;
}

// Insert: O(log log u)
void insert(vEB *V, int x) {
    if (V->min == NIL) {
        emptyTreeInsert(V, x); 
        return; 
    } 
    
    if (x == V->min || x == V->max) {
        return;
    }
    
    if (x < V->min) {
        int tmp = x; 
        x = V->min;
        V->min = tmp;
    } 
    
    if (V->u > 2) {
        int h = high(x, V->u), l = low(x, V->u);
        
        if (V->cluster[h]->min == NIL) {
            insert(V->summary, h);
            emptyTreeInsert(V->cluster[h], l);
        } else {
            insert(V->cluster[h], l);
        } 
    } 
    
    if (x > V->max) {
        V->max = x;
    }
}

// Delete: O(log log u) 
void deleteKey(vEB *V, int x) {
    if (V->min == NIL) {
        return;
    }

    // Single-element tree
    if (V->min == V->max) {
        if (V->min == x) {
            V->min = NIL;
            V->max = NIL;
        }
        
        return;
    } 
    
    // Base case u = 2
    if (V->u <= 2) {
        if (x == 0) {
            V->min = 1;
        } else {
            V->min = 0; 
        }
    
        V->max = V->min; 
        return;
    } 
        
    if (x == V->min) {
        int first_cluster = V->summary->min;
            
        x =indexVal(first_cluster, V->cluster[first_cluster]->min, V->u); 
        V->min = x;
    } 
        
    int h = high(x, V->u), l = low(x, V->u);
    deleteKey(V->cluster[h], l);
        
    if (V->cluster[h]->min == NIL) {
        deleteKey(V->summary, h);
            
        if (x == V->max) {
            int summary_max = V->summary->max;
                
            if (summary_max == NIL) {
                V->max = V->min;
            } else {
                V->max = indexVal(summary_max, V->cluster[summary_max]->max, V->u);
            }
        }
    } else if (x == V->max) {
        V->max = indexVal(h, V->cluster[h]->max, V->u);
    }
}
    
// Visual display of vEB structure without `|-` branch glyphs
void display(vEB *V, int depth, const char *label) {
    for (int i = 0; i < depth; i++) {
        printf(" "); 
    }
    
    printf("%s [u=%d, min=%d, max=%d]\n", label, V->u, V->min, V->max);
    
    if (V->u > 2 && V->summary && V->summary->min != NIL) {
        for (int i = 0; i <= depth; i++) {
            printf(" ");
        } 
        
        printf("Summary: min=%d, max=%d\n", V->summary->min, V->summary->max);
        
        int num_clusters = (int)sqrt(V->u);
        
        for (int i = 0; i < num_clusters; i++) {
            if (V->cluster[i]->min != NIL) {
                char clabel[32];
                snprintf(clabel, sizeof(clabel), "-> Cluster %d", i);
                
                display(V->cluster[i], depth + 1, clabel);
            } 
        } 
    } 
} 

int main() { 
    int universe = 16;
    
    vEB *T = createvEB(universe); 
    
    int choice, x;
    clock_t start, end;
    double time_taken;
    
    while (1) { 
        printf("\n1. Insert Key\n");
        printf("2. Search Member\n");
        printf("3. Successor\n");
        printf("4. Predecessor\n");
        printf("5. Delete Key\n");
        printf("6. Auto-Load Sample Keys (2, 3, 4, 5, 7, 14, 15)\n");
        printf("7. Exit\n");
        
        printf("\nEnter choice: ");
        if (scanf("%d", &choice) != 1 || choice == 7) {
            break;
        }
        
        if (choice == 1) {
            printf("\nEnter key (0 to %d): ", universe- 1); 
            scanf("%d", &x); 
            
            if (x < 0 || x >= universe) { 
                printf("Key out of universe bounds!\n");
                continue; 
            } 
            
            start = clock(); 
            insert(T, x); 
            end = clock(); 
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC; 
            
            printf("\n--- Structure After Inserting %d---\n", x); 
            display(T, 0, "vEB Root"); 
            
            printf("Execution Time: %.2f microseconds\n", time_taken); 
        } else if (choice == 2) { 
            printf("\nEnter key to search: "); 
            scanf("%d", &x);
            
            start = clock(); 
            bool found = isMember(T, x);
            end = clock();
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            printf("Result: Key %d is %s\n", x, found ? "PRESENT" : "NOT PRESENT");
            printf("Execution Time: %.2f microseconds\n", time_taken);
        } else if (choice == 3) { 
            printf("Enter key to find Successor of: ");
            scanf("%d", &x);
            
            start = clock();
            int res = successor(T, x);
            end = clock();
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            
            if (res != NIL) {
                printf("Successor(%d) = %d\n", x, res);
            } else {
                printf("No successor exists for key %d.\n", x); 
            }
            
            printf("Execution Time: %.2f microseconds\n", time_taken);
        } else if (choice == 4) { 
            printf("\nEnter key to find Predecessor of: ");
            scanf("%d", &x);
            
            start = clock(); 
            int res = predecessor(T, x); 
            end = clock(); 
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC; 
            
            if (res != NIL) {
                printf("Predecessor(%d) = %d\n", x, res); 
            } else {
                printf("No predecessor exists for key %d.\n", x);
            }
            
            printf("Execution Time: %.2f microseconds\n", time_taken); 
        } else if (choice == 5) { 
            printf("\nEnter key to delete: "); 
            scanf("%d", &x);
            
            if (!isMember(T, x)) { 
                printf("Key %d does not exist in the tree.\n", x); 
                continue; 
            } 
            
            start = clock(); 
            deleteKey(T, x); 
            end = clock(); 
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC; 
            
            printf("\n--- Structure After Deleting %d---\n", x); 
            display(T, 0, "vEB Root"); 
            
            printf("Execution Time: %.2f microseconds\n", time_taken);
        } else if (choice == 6) {
            int samples[] = {2, 3, 4, 5, 7, 14, 15};
            int count = sizeof(samples) / sizeof(samples[0]); 
            
            start = clock(); 
            for (int i = 0; i < count; i++) {
                insert(T, samples[i]);
            } 
            end = clock(); 
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC; 
            
            printf("\n--- Structure After Loading Sample Keys---\n"); 
            display(T, 0, "vEB Root"); 
            
            printf("Execution Time: %.2f microseconds\n", time_taken);
        }
    }
    
    return 0; 
}
