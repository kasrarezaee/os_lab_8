#include <stdio.h>

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int waitingTime;
    int turnaroundTime;
    int completionTime;
};

int main() {
    int n, i;
    float avgWait = 0, avgTurnaround = 0;
    
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    
    struct Process p[n];
    
    printf("\nEnter process details:\n");
    for(i = 0; i < n; i++) {
        p[i].id = i + 1;
        printf("P%d arrival time: ", i + 1);
        scanf("%d", &p[i].arrivalTime);
        printf("P%d burst time: ", i + 1);
        scanf("%d", &p[i].burstTime);
    }
    
    // Sort by arrival time (FCFS)
    for(i = 0; i < n - 1; i++) {
        for(int j = 0; j < n - i - 1; j++) {
            if(p[j].arrivalTime > p[j+1].arrivalTime) {
                struct Process temp = p[j];
                p[j] = p[j+1];
                p[j+1] = temp;
            }
        }
    }
    
    // Calculate completion, waiting, and turnaround times
    int currentTime = 0;
    
    for(i = 0; i < n; i++) {
        // If process arrives after current time, wait for it
        if(currentTime < p[i].arrivalTime) {
            currentTime = p[i].arrivalTime;
        }
        
        p[i].waitingTime = currentTime - p[i].arrivalTime;
        p[i].completionTime = currentTime + p[i].burstTime;
        p[i].turnaroundTime = p[i].completionTime - p[i].arrivalTime;
        
        currentTime = p[i].completionTime;
    }
    
    // Display results
    int totalWait = 0;
    int totalTurnaround = 0;
    
    printf("\nFCFS Scheduling Results:\n");
    printf("PID\tArrival\tBurst\tWait\tTurnaround\tCompletion\n");
    
    for(i = 0; i < n; i++) {
        totalWait += p[i].waitingTime;
        totalTurnaround += p[i].turnaroundTime;
        
        printf("P%d\t%d\t%d\t%d\t%d\t\t%d\n", 
               p[i].id, p[i].arrivalTime, p[i].burstTime, 
               p[i].waitingTime, p[i].turnaroundTime, p[i].completionTime);
    }
    
    avgWait = (float)totalWait / n;
    avgTurnaround = (float)totalTurnaround / n;
    
    printf("\nAverage waiting time: %.2f\n", avgWait);
    printf("Average turnaround time: %.2f\n", avgTurnaround);
    
    return 0;
}
