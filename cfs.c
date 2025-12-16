#include <stdio.h>
#include <stdlib.h>

// ==========================================
// PART 1: DATA STRUCTURES
// ==========================================

#define PROCESS_COUNT 5
#define NICE_0_LOAD 1024

typedef struct {
    int id;
    int vruntime;
    int residual_duration;
} process;

typedef struct {
    int key;
    void *object;
} mydata;

typedef struct rbnode {
    mydata *data;
    struct rbnode *left;
    struct rbnode *right;
} rbnode;

typedef struct {
    rbnode *root;
    int (*compare)(const void *, const void *);
} rbtree;

rbnode* rb_find_min(rbnode *node);
#define RB_MINIMAL(tree) rb_find_min(tree->root)

// ==========================================
// PART 2: HELPER IMPLEMENTATIONS
// ==========================================

process* create_process(int id, int vruntime, int residual_duration) {
    process* p = (process*)malloc(sizeof(process));
    p->id = id;
    p->vruntime = vruntime;
    p->residual_duration = residual_duration;
    return p;
}

void run_process_for_one_tick(process* p) {
    if (p->residual_duration > 0) p->residual_duration--;
}

int is_terminated(process* p) {
    return p->residual_duration <= 0;
}

mydata* makedata_with_object(int key, void *object) {
    mydata *d = (mydata*)malloc(sizeof(mydata));
    d->key = key;
    d->object = object;
    return d;
}

rbtree* rb_create(int (*compare)(const void *, const void *)) {
    rbtree *t = (rbtree*)malloc(sizeof(rbtree));
    t->root = NULL;
    t->compare = compare;
    return t;
}

rbnode* rb_find_min(rbnode *node) {
    if (node == NULL) return NULL;
    while (node->left != NULL) node = node->left;
    return node;
}

rbnode* rb_insert(rbtree *tree, mydata *data) {
    rbnode *new_node = (rbnode*)malloc(sizeof(rbnode));
    new_node->data = data;
    new_node->left = NULL; new_node->right = NULL;

    if (tree->root == NULL) {
        tree->root = new_node;
        return new_node;
    }

    rbnode *current = tree->root;
    while (1) {
        int cmp = tree->compare(data, current->data);
        if (cmp < 0) {
            if (current->left == NULL) { current->left = new_node; break; }
            current = current->left;
        } else {
            if (current->right == NULL) { current->right = new_node; break; }
            current = current->right;
        }
    }
    return new_node;
}

rbnode* delete_node_recursive(rbnode *root, mydata *data, int (*compare)(const void*, const void*)) {
    if (root == NULL) return root;
    int cmp = compare(data, root->data);
    if (cmp < 0) root->left = delete_node_recursive(root->left, data, compare);
    else if (cmp > 0) root->right = delete_node_recursive(root->right, data, compare);
    else {
        if (root->left == NULL) { rbnode *t = root->right; free(root); return t; }
        else if (root->right == NULL) { rbnode *t = root->left; free(root); return t; }
        rbnode *temp = rb_find_min(root->right);
        root->data = temp->data;
        root->right = delete_node_recursive(root->right, temp->data, compare);
    }
    return root;
}

void rb_delete(rbtree *tree, rbnode *node) {
    if (tree && node) tree->root = delete_node_recursive(tree->root, node->data, tree->compare);
}

// ==========================================
// PART 3: SCHEDULER LOGIC (EXERCISE AREA)
// ==========================================

int compare_func(const void *left, const void *right) {
    int l = ((mydata *)left)->key;
    int r = ((mydata *)right)->key;
    return (l - r);
}

int get_process_weight(int process_id) {
    if (process_id % 2 == 0) return 3072; // High Priority
    return 1024;                          // Normal Priority
}

// students_task1: Implement the logic to calculate how much vruntime increases.
// Formula hint: delta = (time * NICE_0_LOAD * x?) / weight
int calculate_vruntime_delta(int actual_runtime_ticks, int weight) {
    // Delta vruntime is inversely proportional to the weight.
    // Higher weight (priority) = smaller delta = runs more often.
    return (actual_runtime_ticks * NICE_0_LOAD) / weight;
}

void fill_process_array(process* process_array[PROCESS_COUNT]){
    for(int i = 0; i < PROCESS_COUNT; i++)
        process_array[i] = create_process(1000 + i, 0, 10 + i);
}

void insert_one_process_to_rbtree(rbtree *rbt, process* proc){
    // students_task2: Create 'mydata' using proc->vruntime as key and insert into tree
    mydata *data = makedata_with_object(proc->vruntime, proc);
    rb_insert(rbt, data);
}

process* process_of_node(rbnode* node){
    return (process *)((mydata *)(node->data))->object;
}

int main(){
    process *processes[PROCESS_COUNT];
    fill_process_array(processes);

    rbtree *rbt = rb_create(compare_func);
    
    // Initial insertion
    for(int i = 0; i < PROCESS_COUNT; i++)
        insert_one_process_to_rbtree(rbt, processes[i]);

    rbnode *node;
    process *current_proc;
    int current_tick = 0;
    int weight, v_delta;

    // Loop until tree is empty
    while ((node = RB_MINIMAL(rbt))) {
        printf("\n--- Tick %d ---\n", current_tick++);

        // students_task3: Extract process from the node
        current_proc = process_of_node(node);
        
        weight = get_process_weight(current_proc->id);
        printf("Running Process %d (Weight: %d, Vruntime: %d)\n", 
               current_proc->id, weight, current_proc->vruntime);

        run_process_for_one_tick(current_proc);

        // students_task4: Calculate delta and update vruntime
        // We ran for 1 tick
        v_delta = calculate_vruntime_delta(1, weight);
        current_proc->vruntime += v_delta;
        
        printf("  -> New Vruntime: %d\n", current_proc->vruntime);

        // students_task5: Re-schedule logic
        // 1. Remove current node from tree (it has the old key)
        rb_delete(rbt, node);

        // 2. If process is NOT terminated, re-insert it (with the new vruntime key)
        if (!is_terminated(current_proc)) {
            insert_one_process_to_rbtree(rbt, current_proc);
        } else {
            printf("  -> Process %d Finished.\n", current_proc->id);
        }
    }

    printf("\nAll tasks completed.\n");
    return 0;
}