# Non-Preemptive Priority Scheduling - Detailed Line-by-Line Explanation

```c
// Non-preemptive priority scheduling (lower priority value = higher priority)
#include <stdio.h>
```
- **Line 1**: Comment explaining the scheduling algorithm - lower priority numbers mean higher priority
- **Line 2**: Includes the Standard Input/Output library for `printf` and `scanf` functions

```c
// Single process entry
struct Process {
    int id;
    int burstTime;
    int priority;
    int waitingTime;
    int turnaroundTime;
};
```
- **Line 5-12**: Defines the `Process` structure with 5 fields:
  - `id`: Process identifier (P1, P2, etc.)
  - `burstTime`: CPU time required for the process to complete
  - `priority`: Priority level (lower number = higher priority)
  - `waitingTime`: Time process waits in ready queue before execution
  - `turnaroundTime`: Total time from arrival to completion (waiting + burst)

```c
int main() {
    int n, i, j;
    struct Process temp;
    float avgWait = 0, avgTurnaround = 0;
```
- **Line 14-18**: Variable declarations:
  - `n`: Number of processes
  - `i`, `j`: Loop counters
  - `temp`: Temporary variable for swapping during sorting
  - `avgWait`, `avgTurnaround`: Floating-point variables to store averages

```c
    printf("Enter the number of processes: ");
    scanf("%d", &n);
```
- **Line 20-21**: Gets user input for number of processes
- **Example**: If user enters `4`, then `n = 4`

```c
    struct Process p[n];
```
- **Line 23**: Declares an array of `n` processes using Variable Length Array (VLA)
- **Memory layout**: Creates contiguous memory for all process structures

```c
    printf("\nEnter process details:\n");
    for(i = 0; i < n; i++) {
        p[i].id = i + 1;
        printf("P%d burst time: ", i + 1);
        scanf("%d", &p[i].burstTime);
        printf("P%d priority (lower = higher): ", i + 1);
        scanf("%d", &p[i].priority);
    }
```
- **Line 25-32**: Input loop to collect process details:
  - **Line 27**: Sets process ID automatically (1, 2, 3, ...)
  - **Line 28-29**: Gets burst time from user
  - **Line 30-31**: Gets priority value from user
  - **Example input**:
    ```
    Process 1: burst=10, priority=3
    Process 2: burst=5, priority=1  (highest priority)
    Process 3: burst=8, priority=2
    ```

## **SORTING PHASE - Priority Ordering**

```c
    // sort by priority (ascending)
    for(i = 0; i < n - 1; i++) {
        for(j = 0; j < n - i - 1; j++) {
            if(p[j].priority > p[j+1].priority) {
                temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
        }
    }
```
- **Line 35-44**: **Bubble Sort** to arrange processes by priority (ascending):
  - Outer loop (`i`): Runs `n-1` times (passes through array)
  - Inner loop (`j`): Compares adjacent elements
  - **Line 38**: If current process has higher priority number (lower priority) than next
  - **Line 39-41**: Swaps the two processes using `temp`
  - **Visual example** (before sorting):
    ```
    P1: priority=3, P2: priority=1, P3: priority=2
    After sorting:
    P2: priority=1 (highest), P3: priority=2, P1: priority=3 (lowest)
    ```

## **SCHEDULING CALCULATIONS**

```c
    // first process has zero waiting time
    p[0].waitingTime = 0;
    p[0].turnaroundTime = p[0].waitingTime + p[0].burstTime;
```
- **Line 47-49**: Calculates for the first (highest priority) process:
  - Waiting time = 0 (starts immediately)
  - Turnaround time = Waiting time + Burst time
  - **Example**: If P2 has burst=5, then:
    - Waiting = 0
    - Turnaround = 0 + 5 = 5

```c
    // waiting time of each process = turnaround of previous
    for(i = 1; i < n; i++) {
        p[i].waitingTime = p[i-1].turnaroundTime;
        p[i].turnaroundTime = p[i].waitingTime + p[i].burstTime;
    }
```
- **Line 52-55**: Calculates for remaining processes:
  - **Line 53**: Current process waits until previous process finishes
    - `p[i].waitingTime = p[i-1].turnaroundTime`
  - **Line 54**: Turnaround = Waiting + Burst
  - **Example continuation**:
    - After P2 (turnaround=5), P3 with burst=8:
      - Waiting = 5
      - Turnaround = 5 + 8 = 13
    - Then P1 with burst=10:
      - Waiting = 13
      - Turnaround = 13 + 10 = 23

## **RESULT CALCULATION AND DISPLAY**

```c
    int totalWait = 0;
    int totalTurnaround = 0;
```
- **Line 57-58**: Initialize accumulators for totals

```c
    printf("\nPriority scheduling results:\n");
    printf("PID\tPrio\tBurst\tWait\tTurnaround\n");
```
- **Line 60-61**: Prints table header with tabs (`\t`) for alignment

```c
    for(i = 0; i < n; i++) {
        totalWait += p[i].waitingTime;
        totalTurnaround += p[i].turnaroundTime;
        
        printf("P%d\t%d\t%d\t%d\t%d\n", 
             p[i].id, p[i].priority, p[i].burstTime, p[i].waitingTime, p[i].turnaroundTime);
    }
```
- **Line 63-70**: Loop to display results and calculate totals:
  - **Line 64**: Adds current waiting time to total
  - **Line 65**: Adds current turnaround time to total
  - **Line 67-69**: Prints process details in formatted table
  - **Example output**:
    ```
    PID    Prio    Burst    Wait    Turnaround
    P2     1       5        0       5
    P3     2       8        5       13
    P1     3       10       13      23
    ```

```c
    avgWait = (float)totalWait / n;
    avgTurnaround = (float)totalTurnaround / n;
```
- **Line 72-73**: Calculates averages:
  - **`(float)`**: Type casting to get floating-point division (not integer division)
  - **Example**: If totalWait=18 and n=3, avgWait=18/3=6.00

```c
    printf("\nAverage waiting time: %.2f\n", avgWait);
    printf("Average turnaround time: %.2f\n", avgTurnaround);
```
- **Line 75-76**: Displays averages with 2 decimal places (`%.2f`)

```c
    return 0;
}
```
- **Line 78**: Returns 0 indicating successful program termination

---

## **COMPLETE EXAMPLE EXECUTION**

### **User Input:**
```
Enter the number of processes: 3

Enter process details:
P1 burst time: 10
P1 priority (lower = higher): 3
P2 burst time: 5  
P2 priority (lower = higher): 1
P3 burst time: 8
P3 priority (lower = higher): 2
```

### **Internal Processing:**
1. **Initial array**: [P1(prio3), P2(prio1), P3(prio2)]
2. **After sorting**: [P2(prio1), P3(prio2), P1(prio3)]
3. **Calculations**:
   - P2: Wait=0, Turnaround=0+5=5
   - P3: Wait=5, Turnaround=5+8=13  
   - P1: Wait=13, Turnaround=13+10=23
4. **Totals**: Wait=0+5+13=18, Turnaround=5+13+23=41
5. **Averages**: Wait=18/3=6.00, Turnaround=41/3=13.67

### **Output:**
```
Priority scheduling results:
PID     Prio    Burst   Wait    Turnaround
P2      1       5       0       5
P3      2       8       5       13
P1      3       10      13      23

Average waiting time: 6.00
Average turnaround time: 13.67
```

---

## **KEY SCHEDULING CONCEPTS**

1. **Non-Preemptive**: Once a process starts, it runs to completion
2. **Priority-Based**: Lower priority number = higher priority
3. **FCFS within same priority**: Code doesn't handle equal priorities, but bubble sort maintains original order for equal values
4. **No Arrival Time**: All processes assumed to arrive at time 0 (simplification)
5. **Gantt Chart Implicit**: Execution order is P2 → P3 → P1

This is a **simulation** of priority scheduling that calculates metrics but doesn't actually execute processes in real-time. It demonstrates the algorithm's decision-making logic and performance characteristics.