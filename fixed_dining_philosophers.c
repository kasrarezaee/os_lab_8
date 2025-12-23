#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5

sem_t chopstick[N];

void *philosopher(void *num) {
    int id = *(int *)num;
    while (1) {
        printf("Philosopher %d wants to eat\n", id);

         // fix deadlock with this if/else block
        if (id % 2 == 0) {
            // if even: pick left first
            printf("Philosopher %d tries to pick left chopstick\n", id);
            sem_wait(&chopstick[id]);
            printf("Philosopher %d picks the left chopstick\n", id);

            printf("Philosopher %d tries to pick right chopstick\n", id);
            sem_wait(&chopstick[(id+1)%N]);
            printf("Philosopher %d picks the right chopstick\n", id);
        } else {
            // if odd: pick right first
            printf("Philosopher %d tries to pick right chopstick\n", id);
            sem_wait(&chopstick[(id+1)%N]);
            printf("Philosopher %d picks the right chopstick\n", id);

            printf("Philosopher %d tries to pick left chopstick\n", id);
            sem_wait(&chopstick[id]);
            printf("Philosopher %d picks the left chopstick\n", id);
        }

        printf("Philosopher %d begins to eat\n", id);
        sleep(1); // simulate eating

        sem_post(&chopstick[id]);
        sem_post(&chopstick[(id+1)%N]);
        printf("Philosopher %d has finished eating and leaves chopsticks\n", id);

        sleep(1); // simulate thinking
    }
    return NULL;
}

int main() {
    pthread_t thread_id[N];
    int id[N];

    for (int i = 0; i < N; i++)
        sem_init(&chopstick[i], 0, 1);

    for (int i = 0; i < N; i++) {
        id[i] = i;
        pthread_create(&thread_id[i], NULL, philosopher, &id[i]);
    }

    for (int i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);

    return 0;
}
