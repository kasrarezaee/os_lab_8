#include <stdio.h>

struct Process {
    int id;
    int burstTime;
    int remainingTime;
    int waitingTime;
    int turnaroundTime;
    int lastExecutionTime;
};

int main() {
    int n, i, timeQuantum;
    float avgWait = 0, avgTurnaround = 0;
    
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    
    printf("Enter time quantum: ");
    scanf("%d", &timeQuantum);
    
    struct Process p[n];
    
    printf("\nEnter process details:\n");
    for(i = 0; i < n; i++) {
        p[i].id = i + 1;
        printf("P%d burst time: ", i + 1);
        scanf("%d", &p[i].burstTime);
        p[i].remainingTime = p[i].burstTime;
        p[i].waitingTime = 0;
        p[i].lastExecutionTime = 0; // Time when process last executed
    }
    
    // Round Robin Simulation
    int currentTime = 0;
    int completed = 0;
    int totalWait = 0;
    int totalTurnaround = 0;
    
    printf("\nRound Robin Execution Order:\n");
    
    while(completed < n) {
        int allIdle = 1; // Flag to detect idle CPU
        
        for(i = 0; i < n; i++) {
            if(p[i].remainingTime > 0) {
                allIdle = 0; // There's work to do
                
                // Calculate waiting time for this cycle
                if(currentTime > p[i].lastExecutionTime) {
                    p[i].waitingTime += currentTime - p[i].lastExecutionTime;
                }
                
                printf("Time %d-%d: P%d\n", 
                       currentTime, 
                       currentTime + (p[i].remainingTime < timeQuantum ? p[i].remainingTime : timeQuantum),
                       p[i].id);
                
                // Execute process for quantum or remaining time
                int executionTime = (p[i].remainingTime < timeQuantum) ? 
                                    p[i].remainingTime : timeQuantum;
                
                currentTime += executionTime;
                p[i].remainingTime -= executionTime;
                p[i].lastExecutionTime = currentTime;
                
                // If process completed
                if(p[i].remainingTime == 0) {
                    completed++;
                    p[i].turnaroundTime = currentTime;
                    totalWait += p[i].waitingTime;
                    totalTurnaround += p[i].turnaroundTime;
                }
            }
        }
        
        // If no process was executed in this cycle (all completed or idle)
        if(allIdle) {
            currentTime++;
        }
    }
    
    // Display results
    printf("\nRound Robin Scheduling Results (Quantum=%d):\n", timeQuantum);
    printf("PID\tBurst\tWait\tTurnaround\n");
    
    for(i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\n", 
               p[i].id, p[i].burstTime, p[i].waitingTime, p[i].turnaroundTime);
    }
    
    avgWait = (float)totalWait / n;
    avgTurnaround = (float)totalTurnaround / n;
    
    printf("\nAverage waiting time: %.2f\n", avgWait);
    printf("Average turnaround time: %.2f\n", avgTurnaround);
    
    return 0;
}
