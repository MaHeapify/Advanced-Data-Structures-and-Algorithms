/* 
    Aim: Implement a Fibonacci Heap and demonstrate key mergeable heap operations.
    
    Tasks:
        a) Implement the following operations on a Fibonacci Heap:
            Insert(x) - Insert a new node
            Find-Min() - Return the minimum key
            Union(H1, H2) - Merge two Fibonacci Heaps
        b) Display the heap structure before and after each operation.
        
    Expected Output:
        Display of heap structure after each insert and merge operation.
        Verification of the minimum key after each step.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct FibNode {
    int key; 
    struct FibNode *left, *right, *child, *parent; 
} FibNode;

typedef struct {
    FibNode *min; int n;
} FibHeap;

FibHeap* createHeap() {
    return (FibHeap*)calloc(1, sizeof(FibHeap));
}

FibNode* createNode(int val) {
    FibNode *node = (FibNode*)calloc(1, sizeof(FibNode));
    node->key = val;
    node->left = node->right = node;
    return node;
}

void display(FibHeap *H, const char *name) {
    printf("\n[%s] Total Nodes: %d\n", name, H->n);
    if (!H->min) { 
        printf("Heap is empty.\n"); 
        return;
    }

    printf("Root List: ");
    FibNode *curr = H->min;
    
    do { 
        printf("[%d] <-> ", curr->key);
        curr = curr->right;
    } while (curr != H->min);
    
    printf("(circular)\nMinimum Key: %d\n", H->min->key);
}

void insert(FibHeap *H, int val) {
    FibNode *x = createNode(val);
    
    if (!H->min) {
        H->min = x;
    } else { 
        x->right = H->min->right;
        x->left = H->min;
        H->min->right->left = x;
        H->min->right = x;
        
        if (x->key < H->min->key) {
            H->min = x;
        } 
    } 
        
    H->n++;
}
    
FibHeap* heapUnion(FibHeap *H1, FibHeap *H2) {
    FibHeap *H = createHeap();
    H->min = H1->min;
    if (H1->min && H2->min) {
        FibNode *r1 = H1->min->right, *l2 = H2->min->left;
        H1->min->right = H2->min; H2->min->left = H1->min;
        r1->left = l2; l2->right = r1;
        
        if (H2->min->key < H1->min->key) {
            H->min = H2->min; 
        }
    } else if (!H1->min) {
        H->min = H2->min;
    }
    H->n = H1->n + H2->n;
    H1->min = H2->min = NULL;
    H1->n = H2->n = 0;
    return H;
}

int main() {
    FibHeap *H1 = createHeap(), *H2 = createHeap();
    int choice, val, target;
    clock_t start, end;
    double time_taken;
    while (1) {
        printf("\n1. Insert into Heap 1\n");
        printf("2. Insert into Heap 2\n");
        printf("3. Find-Min\n");
        printf("4. Union(Heap 1, Heap 2)\n");
        printf("5. Exit\n");
        
        printf("\nEnter choice: ");
        if (scanf("%d", &choice) != 1 || choice == 5) {
            break; 
        }
        
        if (choice == 1 || choice == 2) {
            printf("\nEnter value to insert: ");
            scanf("%d", &val);
        
            FibHeap *th = (choice == 1) ? H1 : H2;
            const char *hname = (choice == 1) ? "Heap 1" : "Heap 2";
            
            start = clock();
            insert(th, val);
            end = clock();

            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            display(th, hname);

            printf("Execution Time: %.2f microseconds\n", time_taken);
        } else if (choice == 3) {
            printf("\nEnter heap to search (1 or 2): ");
            scanf("%d", &target);

            FibHeap *th = (target == 1) ? H1 : H2;
            start = clock();
            int min_val = th->min ? th->min->key :-1;
            end = clock();
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            
            if (min_val !=-1) {
                printf("\nMinimum key in Heap %d: %d\n", target, min_val);
            } else {
                printf("\nHeap %d is empty.\n", target);
            }
            
            printf("Execution Time: %.2f microseconds\n", time_taken);
        } else if (choice == 4) {
            start = clock();
            H1 = heapUnion(H1, H2);
            end = clock();
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            
            display(H1, "Merged (Heap 1)");
            
            printf("Execution Time: %.2f microseconds\n", time_taken);
        }
    }
    
    return 0;
}
