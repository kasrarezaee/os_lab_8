/* ============================================================
   Banker's Algorithm Implementation - SAFE INITIAL STATE VERSION
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUMBER_OF_RESOURCES 5
#define NUMBER_OF_CUSTOMERS 5

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

pthread_mutex_t mutex_lock;
int running = 1;

/* ============================================================
   SAFE MAXIMUM MATRIX CONFIGURATION
   With available = [10, 5, 7, 3, 2], this ensures safe state
   ============================================================ */
void initialize_system(int argc, char *argv[]) {
    if (argc < NUMBER_OF_RESOURCES + 1) {
        printf("Error: Not enough arguments. Provide %d resource values.\n", NUMBER_OF_RESOURCES);
        printf("Usage: %s", argv[0]);
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            printf(" r%d", i + 1);
        }
        printf("\n");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = strtol(argv[i + 1], NULL, 10);
        printf("Resource R%d available: %d\n", i + 1, available[i]);
    }
    
    /* SAFE CONFIGURATION: Total needs <= Total resources */
    /* Total resources: [10, 5, 7, 3, 2] */
    /* SAFE maximum matrix: */
    /* CORRECTED SAFE MAXIMUM MATRIX */
    int max_matrix[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] = {
        /* Customer 0: Reduced R1 from 3 to 2 */
        {2, 2, 1, 1, 1},  /* Total: 7 of available 10 */
    
        /* Customer 1: Reduced R1 from 2 to 1 */
        {1, 1, 2, 1, 1},  /* Total: 6 of available 10 */
    
        /* Customer 2: Keep same */
        {4, 1, 2, 0, 0},  /* Doesn't need R4 and R5 */
    
        /* Customer 3: Keep same */
        {1, 0, 1, 1, 0},  /* Very small needs */
    
        /* Customer 4: Reduced R1 from 2 to 2 (keep same) */
        {2, 1, 1, 0, 0}   /* Doesn't need all resources */
    };
    
    /* VERIFY: Total maximum demand should be <= total resources */
    int total_max[NUMBER_OF_RESOURCES] = {0};
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            maximum[i][j] = max_matrix[i][j];
            total_max[j] += max_matrix[i][j];
        }
    }
    
    printf("\nTotal maximum demand per resource: ");
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        printf("R%d: %d/%d  ", j+1, total_max[j], available[j]);
    }
    printf("\n");
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            allocation[i][j] = 0;
        }
    }
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
    
    printf("System initialized successfully!\n");
}

void print_state() {
    printf("\n=== CURRENT SYSTEM STATE ===\n");
    
    printf("Available resources: ");
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        printf("R%d: %d  ", j + 1, available[j]);
    }
    printf("\n\n");
    
    printf("Customer\tMax\t\tAllocation\tNeed\n");
    printf("--------\t---\t\t----------\t----\n");
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("C%d\t\t", i);
        
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", maximum[i][j]);
        }
        printf("\t");
        
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", allocation[i][j]);
        }
        printf("\t");
        
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }
    printf("=============================\n\n");
}

int is_safe() {
    int work[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS];
    
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        finish[i] = 0;
    }
    
    int found;
    for (int count = 0; count < NUMBER_OF_CUSTOMERS; count++) {
        found = 0;
        
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (finish[i] == 0) {
                int can_allocate = 1;
                
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = 0;
                        break;
                    }
                }
                
                if (can_allocate) {
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                    
                    finish[i] = 1;
                    found = 1;
                    break;
                }
            }
        }
        
        if (!found) {
            return 0;
        }
    }
    
    return 1;
}

int request_resources(int customer_num, int request[]) {
    printf("\nCustomer %d requesting: ", customer_num);
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (request[j] > 0) printf("R%d:%d ", j + 1, request[j]);
    }
    printf("\n");
    
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (request[j] > need[customer_num][j]) {
            printf("  ERROR: Request exceeds need\n");
            return -1;
        }
    }
    
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (request[j] > available[j]) {
            printf("  Resources not available. Waiting...\n");
            return -1;
        }
    }
    
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        available[j] -= request[j];
        allocation[customer_num][j] += request[j];
        need[customer_num][j] -= request[j];
    }
    
    if (is_safe()) {
        printf("  Request GRANTED to Customer %d\n", customer_num);
        return 0;
    } else {
        printf("  Request DENIED to Customer %d (unsafe)\n", customer_num);
        
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            available[j] += request[j];
            allocation[customer_num][j] -= request[j];
            need[customer_num][j] += request[j];
        }
        
        return -1;
    }
}

int release_resources(int customer_num, int release[]) {
    printf("\nCustomer %d releasing: ", customer_num);
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (release[j] > 0) printf("R%d:%d ", j + 1, release[j]);
    }
    printf("\n");
    
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        if (release[j] > allocation[customer_num][j]) {
            printf("  ERROR: Cannot release more than allocated\n");
            return -1;
        }
    }
    
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
        available[j] += release[j];
        allocation[customer_num][j] -= release[j];
        need[customer_num][j] = maximum[customer_num][j] - allocation[customer_num][j];
    }
    
    printf("  Resources released successfully\n");
    return 0;
}

void* customer_thread(void* arg) {
    int customer_id = *(int*)arg;
    unsigned int seed = time(NULL) + customer_id;
    
    printf("Customer %d started\n", customer_id);
    
    while (running) {
        /* Generate SMART requests that are more likely to succeed */
        int request[NUMBER_OF_RESOURCES] = {0};
        
        /* Strategy: Request small amounts, not everything at once */
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (need[customer_id][j] > 0) {
                /* Request 0-50% of remaining need, but max 2 of any resource */
                int max_request = need[customer_id][j];
                if (max_request > 2) max_request = 2;
                request[j] = rand_r(&seed) % (max_request + 1);
            }
        }
        
        /* Don't make empty requests */
        int all_zero = 1;
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (request[j] > 0) {
                all_zero = 0;
                break;
            }
        }
        if (all_zero) {
            usleep(200000);
            continue;
        }
        
        pthread_mutex_lock(&mutex_lock);
        int result = request_resources(customer_id, request);
        pthread_mutex_unlock(&mutex_lock);
        
        if (result == 0) {
            printf("Customer %d using resources...\n", customer_id);
            
            /* Use resources for 1-4 seconds */
            int use_time = 1 + (rand_r(&seed) % 4);
            sleep(use_time);
            
            pthread_mutex_lock(&mutex_lock);
            release_resources(customer_id, request);
            pthread_mutex_unlock(&mutex_lock);
            
            printf("Customer %d finished using resources\n", customer_id);
        } else {
            printf("Customer %d request denied\n", customer_id);
        }
        
        /* Wait 1-3 seconds before next request */
        int delay = 1 + (rand_r(&seed) % 3);
        sleep(delay);
    }
    
    printf("Customer %d exiting\n", customer_id);
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("\n============ BANKER'S ALGORITHM SIMULATION ============\n");
    
    /* Use default values if no arguments provided */
    if (argc == 1) {
        printf("Using default resource values: 10 5 7 3 2\n");
        char* default_args[] = {"./banker", "10", "5", "7", "3", "2", NULL};
        initialize_system(6, default_args);
    } else {
        initialize_system(argc, argv);
    }
    
    if (pthread_mutex_init(&mutex_lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return EXIT_FAILURE;
    }
    
    print_state();
    
    printf("\nChecking initial system safety...\n");
    pthread_mutex_lock(&mutex_lock);
    int initial_safe = is_safe();
    pthread_mutex_unlock(&mutex_lock);
    
    if (!initial_safe) {
        printf("ERROR: Initial system state is unsafe!\n");
        return EXIT_FAILURE;
    }
    
    printf("Initial state is SAFE ✓\n");
    
    pthread_t customers[NUMBER_OF_CUSTOMERS];
    int customer_ids[NUMBER_OF_CUSTOMERS];
    
    printf("\nCreating %d customer threads...\n", NUMBER_OF_CUSTOMERS);
    
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        customer_ids[i] = i;
        if (pthread_create(&customers[i], NULL, customer_thread, &customer_ids[i]) != 0) {
            printf("Failed to create thread for customer %d\n", i);
            return EXIT_FAILURE;
        }
    }
    
    printf("\n=== SIMULATION STARTED ===\n");
    printf("Running for 30 seconds...\n\n");
    
    /* Run simulation */
    for (int t = 0; t < 30; t++) {
        printf("[Time: %02d sec] ", t);
        fflush(stdout);
        sleep(1);
    }
    
    printf("\n\n=== STOPPING SIMULATION ===\n");
    running = 0;
    
    /* Wait for threads */
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
        printf("Customer %d joined\n", i);
    }
    
    pthread_mutex_destroy(&mutex_lock);
    
    printf("\n=== FINAL STATE ===\n");
    print_state();
    
    pthread_mutex_lock(&mutex_lock);
    printf("Final safety check: ");
    int final_safe = is_safe();
    pthread_mutex_unlock(&mutex_lock);
    
    if (final_safe) {
        printf("SAFE ✓\n\nSUCCESS: Simulation completed without deadlock!\n");
    } else {
        printf("UNSAFE ✗\n\nWARNING: Simulation ended in unsafe state!\n");
    }
    
    printf("\n============ SIMULATION COMPLETE ============\n");
    return EXIT_SUCCESS;
}