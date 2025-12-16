# Red-Black Tree Scheduler - Detailed Line-by-Line Explanation

## **PART 1: DATA STRUCTURES**

```c
#include <stdio.h>
#include <stdlib.h>
```
- **Line 1-2**: Include standard C libraries for I/O operations (`stdio.h`) and dynamic memory allocation (`stdlib.h`).

```c
#define PROCESS_COUNT 5
#define NICE_0_LOAD 1024
```
- **Line 4-5**: Define constants:
  - `PROCESS_COUNT`: Number of processes to simulate (5)
  - `NICE_0_LOAD`: Base weight/priority value (1024) used in vruntime calculations

```c
typedef struct {
    int id;
    int vruntime;
    int residual_duration;
} process;
```
- **Line 7-11**: Define a `process` struct representing a CPU process:
  - `id`: Unique identifier for the process
  - `vruntime`: Virtual runtime - tracks how much "CPU time" the process has consumed (adjusted by priority)
  - `residual_duration`: How many time units the process still needs to complete

```c
typedef struct {
    int key;
    void *object;
} mydata;
```
- **Line 13-16**: Define a `mydata` struct for tree node data:
  - `key`: The value used for ordering in the Red-Black Tree (vruntime in this scheduler)
  - `object`: Generic pointer to any data (will point to a `process`)

```c
typedef struct rbnode {
    mydata *data;
    struct rbnode *left;
    struct rbnode *right;
} rbnode;
```
- **Line 18-22**: Define a binary tree node structure:
  - `data`: Pointer to the data stored in this node
  - `left`: Pointer to left child (smaller keys)
  - `right`: Pointer to right child (larger keys)
  - *Note: This is actually a Binary Search Tree (BST), not a full Red-Black Tree*

```c
typedef struct {
    rbnode *root;
    int (*compare)(const void *, const void *);
} rbtree;
```
- **Line 24-28**: Define the tree structure:
  - `root`: Pointer to the root node of the tree
  - `compare`: Function pointer to a comparison function (enables generic sorting)

```c
rbnode* rb_find_min(rbnode *node);
#define RB_MINIMAL(tree) rb_find_min(tree->root)
```
- **Line 30-31**: Forward declaration of `rb_find_min` function and a macro that finds the minimum element (leftmost node) in a tree.

---

## **PART 2: HELPER IMPLEMENTATIONS**

### **Process Management Functions**

```c
process* create_process(int id, int vruntime, int residual_duration) {
    process* p = (process*)malloc(sizeof(process));
    p->id = id;
    p->vruntime = vruntime;
    p->residual_duration = residual_duration;
    return p;
}
```
- **Line 36-42**: Creates a new process dynamically:
  - Allocates memory for a process struct
  - Initializes all fields with provided values
  - Returns pointer to the created process

```c
void run_process_for_one_tick(process* p) {
    if (p->residual_duration > 0) p->residual_duration--;
}
```
- **Line 44-46**: Simulates running a process for one time unit:
  - Decreases the remaining duration by 1 (if not already finished)

```c
int is_terminated(process* p) {
    return p->residual_duration <= 0;
}
```
- **Line 48-50**: Checks if a process has finished execution

### **Tree Data Structure Functions**

```c
mydata* makedata_with_object(int key, void *object) {
    mydata *d = (mydata*)malloc(sizeof(mydata));
    d->key = key;
    d->object = object;
    return d;
}
```
- **Line 52-57**: Creates a data wrapper for tree nodes:
  - Allocates memory for `mydata`
  - Stores the key and object pointer
  - Used to store processes in the tree with vruntime as key

```c
rbtree* rb_create(int (*compare)(const void *, const void *)) {
    rbtree *t = (rbtree*)malloc(sizeof(rbtree));
    t->root = NULL;
    t->compare = compare;
    return t;
}
```
- **Line 59-64**: Creates an empty tree:
  - Allocates memory for tree structure
  - Initializes root to NULL (empty tree)
  - Stores the comparison function for ordering

```c
rbnode* rb_find_min(rbnode *node) {
    if (node == NULL) return NULL;
    while (node->left != NULL) node = node->left;
    return node;
}
```
- **Line 66-70**: Finds the node with minimum key in a subtree:
  - Traverses left children until reaching the leftmost node
  - Returns NULL if tree is empty

```c
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
```
- **Line 72-95**: Standard Binary Search Tree insertion:
  - Creates a new node with the given data
  - If tree is empty, makes new node the root
  - Otherwise, traverses the tree to find correct position
  - Uses comparison function to determine left/right placement
  - Inserts as leaf node

```c
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
```
- **Line 97-112**: Recursive BST deletion with three cases:
  1. **No children**: Simply remove node
  2. **One child**: Replace node with its child
  3. **Two children**: Find inorder successor (minimum in right subtree), copy its data, then delete successor

```c
void rb_delete(rbtree *tree, rbnode *node) {
    if (tree && node) tree->root = delete_node_recursive(tree->root, node->data, tree->compare);
}
```
- **Line 114-116**: Wrapper function for deletion that calls the recursive implementation

---

## **PART 3: SCHEDULER LOGIC**

### **Comparison and Priority Functions**

```c
int compare_func(const void *left, const void *right) {
    int l = ((mydata *)left)->key;
    int r = ((mydata *)right)->key;
    return (l - r);
}
```
- **Line 121-125**: Comparison function for ordering tree nodes:
  - Extracts keys from two `mydata` structures
  - Returns negative if left < right, 0 if equal, positive if left > right
  - Enables ascending order sorting in the tree

```c
int get_process_weight(int process_id) {
    if (process_id % 2 == 0) return 3072; // High Priority
    return 1024;                          // Normal Priority
}
```
- **Line 127-130**: Determines process priority based on ID:
  - Even IDs: High priority (weight 3072)
  - Odd IDs: Normal priority (weight 1024)
  - Higher weight = higher priority = smaller vruntime increment

```c
int calculate_vruntime_delta(int actual_runtime_ticks, int weight) {
    return (actual_runtime_ticks * NICE_0_LOAD) / weight;
}
```
- **Line 136-140**: Calculates vruntime increase after running:
  - `delta = (actual_runtime × base_weight) / process_weight`
  - **Higher weight processes get smaller vruntime increases**
  - This is key to Completely Fair Scheduler (CFS) algorithm

### **Process and Tree Setup**

```c
void fill_process_array(process* process_array[PROCESS_COUNT]){
    for(int i = 0; i < PROCESS_COUNT; i++)
        process_array[i] = create_process(1000 + i, 0, 10 + i);
}
```
- **Line 142-145**: Creates an array of 5 processes:
  - IDs: 1000, 1001, 1002, 1003, 1004
  - Initial vruntime: 0 for all
  - Residual duration: 10, 11, 12, 13, 14 respectively

```c
void insert_one_process_to_rbtree(rbtree *rbt, process* proc){
    mydata *data = makedata_with_object(proc->vruntime, proc);
    rb_insert(rbt, data);
}
```
- **Line 147-151**: Inserts a process into the tree:
  - Creates a `mydata` wrapper with `vruntime` as key
  - Inserts wrapper into the tree (sorted by vruntime)

```c
process* process_of_node(rbnode* node){
    return (process *)((mydata *)(node->data))->object;
}
```
- **Line 153-155**: Extracts process pointer from a tree node:
  - Navigates: `node → data → object → process`

### **Main Scheduling Loop**

```c
int main(){
    process *processes[PROCESS_COUNT];
    fill_process_array(processes);

    rbtree *rbt = rb_create(compare_func);
    
    for(int i = 0; i < PROCESS_COUNT; i++)
        insert_one_process_to_rbtree(rbt, processes[i]);
```
- **Line 157-165**: Initial setup:
  1. Create array of 5 processes
  2. Create empty tree with comparison function
  3. Insert all processes into tree (key = initial vruntime = 0)

```c
    rbnode *node;
    process *current_proc;
    int current_tick = 0;
    int weight, v_delta;

    while ((node = RB_MINIMAL(rbt))) {
        printf("\n--- Tick %d ---\n", current_tick++);
```
- **Line 167-173**: Main scheduling loop:
  - Continues until tree is empty (all processes finished)
  - Each iteration gets the process with **smallest vruntime** (leftmost node)
  - This implements CFS: always run process with least vruntime

```c
        current_proc = process_of_node(node);
        
        weight = get_process_weight(current_proc->id);
        printf("Running Process %d (Weight: %d, Vruntime: %d)\n", 
               current_proc->id, weight, current_proc->vruntime);

        run_process_for_one_tick(current_proc);
```
- **Line 176-181**: Execute selected process:
  1. Extract process from tree node
  2. Determine its weight (priority)
  3. Print status information
  4. Run for 1 time unit (decrease residual duration)

```c
        v_delta = calculate_vruntime_delta(1, weight);
        current_proc->vruntime += v_delta;
        
        printf("  -> New Vruntime: %d\n", current_proc->vruntime);
```
- **Line 184-187**: Update vruntime:
  - Calculate increase based on actual runtime (1 tick) and weight
  - Higher weight = smaller increase = will be selected again sooner

```c
        rb_delete(rbt, node);

        if (!is_terminated(current_proc)) {
            insert_one_process_to_rbtree(rbt, current_proc);
        } else {
            printf("  -> Process %d Finished.\n", current_proc->id);
        }
```
- **Line 190-197**: Rescheduling logic:
  1. Remove process from tree (old vruntime key is no longer valid)
  2. If process still has work remaining, re-insert with **new vruntime**
  3. If finished, don't re-insert (process completes)

```c
    printf("\nAll tasks completed.\n");
    return 0;
}
```
- **Line 199-201**: Cleanup and exit

---

## **KEY SCHEDULING CONCEPTS IMPLEMENTED**

1. **Completely Fair Scheduler (CFS) Algorithm**:
   - Always runs process with smallest vruntime
   - Vruntime increases slower for high-priority processes

2. **Priority-based Fairness**:
   - Processes with weight 3072 (high priority) get vruntime increase of 1024/3072 ≈ 0.33 per tick
   - Processes with weight 1024 (normal priority) get vruntime increase of 1024/1024 = 1 per tick
   - High priority processes run approximately 3x more often

3. **Self-balancing Property**:
   - By always removing and re-inserting processes with updated keys, the tree naturally maintains ordering
   - The leftmost node always has the smallest vruntime

4. **Termination Detection**:
   - Process finishes when residual_duration reaches 0
   - Finished processes are not re-inserted into the tree

This code implements a simplified version of the Linux CFS scheduler using a binary search tree to always select the process with the smallest vruntime for execution.