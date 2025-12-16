// Non-preemptive priority scheduling (lower priority value = higher priority)
#include <stdio.h>

// Single process entry
struct Process {
    int id;
    int burstTime;
    int priority;
    int waitingTime;
    int turnaroundTime;
};

int main() {
    int n, i, j;
    struct Process temp;
    float avgWait = 0, avgTurnaround = 0;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    struct Process p[n];

    printf("\nEnter process details:\n");
    for(i = 0; i < n; i++) {
        p[i].id = i + 1;
        printf("P%d burst time: ", i + 1);
        scanf("%d", &p[i].burstTime);
        printf("P%d priority (lower = higher): ", i + 1);
        scanf("%d", &p[i].priority);
    }

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

    // first process has zero waiting time
    p[0].waitingTime = 0;
    p[0].turnaroundTime = p[0].waitingTime + p[0].burstTime;

    // waiting time of each process = turnaround of previous
    for(i = 1; i < n; i++) {
        p[i].waitingTime = p[i-1].turnaroundTime;
        p[i].turnaroundTime = p[i].waitingTime + p[i].burstTime;
    }

    int totalWait = 0;
    int totalTurnaround = 0;

    printf("\nPriority scheduling results:\n");
    printf("PID\tPrio\tBurst\tWait\tTurnaround\n");

    for(i = 0; i < n; i++) {
        totalWait += p[i].waitingTime;
        totalTurnaround += p[i].turnaroundTime;
        
         printf("P%d\t%d\t%d\t%d\t%d\n", 
             p[i].id, p[i].priority, p[i].burstTime, p[i].waitingTime, p[i].turnaroundTime);
    }

    avgWait = (float)totalWait / n;
    avgTurnaround = (float)totalTurnaround / n;

    printf("\nAverage waiting time: %.2f\n", avgWait);
    printf("Average turnaround time: %.2f\n", avgTurnaround);

    return 0;
}