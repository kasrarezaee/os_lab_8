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

        printf("Philosopher %d tries to pick left chopstick\n", id);
        sem_wait(&chopstick[id]);
        printf("Philosopher %d picks the left chopstick\n", id);

        printf("Philosopher %d tries to pick the right chopstick\n", id);
        sem_wait(&chopstick[(id+1)%N]);
        printf("Philosopher %d picks the right chopstick\n", id);

        printf("Philosopher %d begins to eat\n", id);
        sleep(1); // simulate eating

        printf("Philosopher %d has finished eating\n", id);
        sem_post(&chopstick[id]);
        printf("Philosopher %d leaves the left chopstick\n", id);
        sem_post(&chopstick[(id+1)%N]);
        printf("Philosopher %d leaves the right chopstick\n", id);

        sleep(1); // simulate thinking
    }
    return NULL;
}

int main() {
    pthread_t thread_id[N];
    int i;
    int id[N];

    for (i = 0; i < N; i++)
        sem_init(&chopstick[i], 0, 1);

    for (i = 0; i < N; i++) {
        id[i] = i;
        pthread_create(&thread_id[i], NULL, philosopher, &id[i]);
    }

    for (i = 0; i < N; i++)
        pthread_join(thread_id[i], NULL);

    return 0;
}
