/*
    Aim: Demonstrate decrease-key and delete-node operations in a Fibonacci Heap.
    
    Tasks:
        a) Construct a Fibonacci Heap with at least 10 nodes.
        b) Perform the following operations sequentially:
            Decrease a key (use cascading cuts if required).
            Deletea node by decreasing its key to -∞ and performing Extract-Min().
        c) Print the heap structure after each step.
            
    Expected Output: 
        Intermediate states of the heap showing cascading cuts. 
        Finalheap after deletion.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#define INF_NEG -999999

typedef struct FibNode {
    int key, degree;
    bool mark;
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
    node->key = val; node->left = node->right = node;
    return node;
} 

// Clean hierarchy printer without branch characters
void printTree(FibNode *node, int depth) {
    if (!node) {
        return;
    }

    FibNode *cur = node;
    do {
        for (int i = 0; i < depth; i++) {
            printf(" ");
        }
        
        if (depth == 0) {
            printf("Root: [%d (deg:%d%s)]\n", cur->key, cur->degree, cur->mark ? ", marked" : "");
        } else { 
            printf("-> Child: [%d (deg:%d%s)]\n", cur->key, cur->degree, cur->mark ? ", marked" : "");
        }
        
        if (cur->child) {
            printTree(cur->child, depth + 1);
            
        }
        cur = cur->right;
    } while (cur != node);
}
    
void display(FibHeap *H) {
    printf("\n--- Heap Structure (Nodes: %d)---\n", H->n);
    
    if (!H->min) {
        printf("Heap is empty.\n");
        return;
    }
    
    printf("Minimum Key: %d\nRoot List & Trees:\n", H->min->key);
    printTree(H->min, 0);
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
    
FibNode* findNode(FibNode *root, int val) {
    if (!root) {
        return NULL;
    }
    
    FibNode *curr = root;
    do {
        if (curr->key == val) {
            return curr;
        }
        
        FibNode *res = findNode(curr->child, val);
        
        if (res) {
            return res;
        } 
        curr = curr->right;
    } while (curr != root);
    
    return NULL;
}

void fibLink(FibNode *y, FibNode *x) {
    y->left->right = y->right;
    y->right->left = y->left;
    y->parent = x;
    
    if (!x->child) {
        x->child = y;
        y->left = y->right = y;
    } else {
        y->right = x->child->right;
        y->left = x->child;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
    y->mark = false;
}

void consolidate(FibHeap *H) {
    int max_deg = (int)(log2(H->n)) + 2;
    FibNode **A = (FibNode**)calloc(max_deg, sizeof(FibNode*));
    FibNode *nodes[H->n + 10];
    int count = 0;
    FibNode *cur = H->min;
    
    if (cur) {
        do {
            nodes[count++] = cur;
            cur = cur->right;
        } while (cur != H->min);
    }
    
    for (int i = 0; i < count; i++) {
        FibNode *x = nodes[i];
        int d = x->degree;
        
        while (A[d]) {
            FibNode *y = A[d];
            
            if (x->key > y->key) {
                FibNode *tmp = x;
                x = y;
                y = tmp;
            } 
            
            fibLink(y, x);
            A[d] = NULL;
            d++;
        } 
        A[d] = x;
    }
    H->min = NULL;
    
    for (int i = 0; i < max_deg; i++) {
        if (A[i]) {
            if (!H->min) {
                H->min = A[i];
                A[i]->left = A[i]->right = A[i];
            } else {
                A[i]->right = H->min->right;
                A[i]->left = H->min;
                H->min->right->left = A[i];
                H->min->right = A[i];
                if (A[i]->key < H->min->key) {
                    H->min = A[i];
                }
            } 
        }
    }
    free(A);
} 

FibNode* extractMin(FibHeap *H) {
    FibNode *z = H->min;
    
    if (!z) {
        return NULL;
    }
    
    if (z->child) {
        FibNode *c = z->child;
        do {
            c->parent = NULL;
            c =c->right;
        } while (c != z->child);
        
        FibNode *l1 = z->left, *l2 = z->child->left;
        l1->right = z->child; z->child->left = l1;
        z->right->left = l2;
        l2->right = z->right;
    }
    z->left->right = z->right;
    z->right->left = z->left;
    if (z == z->right) {
        H->min = NULL;
    } else {
        H->min = z->right;
        consolidate(H);
    } 
    H->n--; 
    
    return z;
}

void cut(FibHeap *H, FibNode *x, FibNode *y) {
    if (x == x->right) {
        y->child = NULL;
    } else {
        x->left->right = x->right;
        x->right->left = x->left;
        if (y->child == x) {
            y->child = x->right;
        } 
    } 
    y->degree--;
    x->right = H->min->right;
    x->left = H->min;
    H->min->right->left = x;
    H->min->right = x;
    x->parent = NULL;
    x->mark = false;
}

void cascadingCut(FibHeap *H, FibNode *y) {
    FibNode *z = y->parent;
    if (z) {
        if (!y->mark) {
            y->mark = true;
        } else {
            cut(H, y, z); 
            cascadingCut(H, z);
        }
    }
}

void decreaseKey(FibHeap *H, FibNode *x, int k) {
    if (k > x->key) {
        printf("New key is greater than current key!\n");
        return;
    } 
    x->key = k;
    FibNode *y = x->parent;
    
    if (y && x->key < y->key) {
        cut(H, x, y);
        cascadingCut(H, y);
    } 
    
    if (x->key < H->min->key) {
        H->min = x;
    } 
} 

void deleteNode(FibHeap *H, int val) {
    FibNode *x = findNode(H->min, val); 
    
    if (!x) {
        printf("Key %d not found in heap!\n", val);
        return;
    }
    
    decreaseKey(H, x, INF_NEG);
    FibNode *z = extractMin(H);
    if (z) {
        free(z);
    } 
} 

int main() { 
    FibHeap *H = createHeap();
    int choice, val, new_val;
    clock_t start, end;
    double time_taken;
    
    while (1) {
        printf("\n1. Insert Node\n");
        printf("2. Extract-Min (Consolidate)\n");
        printf("3. Decrease Key\n");
        printf("4. Delete Node\n");
        printf("5. Auto-Load 10 Sample Nodes\n");
        printf("6. Exit\n");
        
        printf("\nEnter choice: ");
        if (scanf("%d", &choice) != 1 || choice == 6) {
            break;
        }
        
        if (choice == 1) {
            printf("\nEnter value to insert: ");
            scanf("%d", &val);
            
            start = clock();
            insert(H, val);
            end = clock();
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            
            display(H); 
            
            printf("Execution Time: %.2f microseconds\n", time_taken);
        } else if (choice == 2) { 
            start = clock(); 
            FibNode *m = extractMin(H);
            end = clock();
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            
            if (m) {
                printf("\nExtracted Min Key: %d\n", m->key);
            } 
            
            display(H); 
            printf("Execution Time: %.2f microseconds\n", time_taken);
            
            if (m) {
                free(m);
            }
        } else if (choice == 3) {
            printf("\nEnter key to decrease: ");
            scanf("%d", &val);
            printf("Enter new smaller key: ");
            scanf("%d", &new_val);
            FibNode *node = findNode(H->min, val);
            
            if (!node) {
                printf("Key %d not found!\n", val);
                continue;
            } 
            
            start = clock();
            decreaseKey(H, node, new_val);
            end = clock();
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            display(H);
            printf("Execution Time: %.2f microseconds\n", time_taken);
        } else if (choice == 4) { 
            printf("\nEnter key to delete: ");
            scanf("%d", &val); 
            
            start = clock();
            deleteNode(H, val); 
            end = clock();
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            display(H);
            
            printf("Execution Time: %.2f microseconds\n", time_taken);
        } else if (choice == 5) {
            int samples[] = {10, 20, 5, 15, 30, 25, 12, 45, 8, 40};
            start = clock(); 
            for (int i = 0; i < 10; i++) {
                insert(H, samples[i]);
            }
            end = clock();
            
            time_taken = ((double)(end- start) * 1000000.0) / CLOCKS_PER_SEC;
            printf("\n10 Sample Nodes Inserted.\n");
            display(H);
            printf("Execution Time: %.2f microseconds\n", time_taken);
        }
    }
    
    return 0; 
}
