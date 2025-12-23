#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

void *fun1(void *arg);
void *fun2(void *arg);

int shared = 1;
sem_t s;

int main()
{
    pthread_t thread1, thread2;

    sem_init(&s, 0, 1);

    pthread_create(&thread1, NULL, fun1, NULL);
    pthread_create(&thread2, NULL, fun2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Final value of shared is %d\n", shared);
    return 0;
}

void *fun1(void *arg)
{
    int x;
    sem_wait(&s);

    x = shared;
    printf("Thread1 reads: %d\n", x);
    x++;
    sleep(1);
    shared = x;
    printf("Thread1 updates shared to %d\n", shared);

    sem_post(&s);
    return NULL;
}

void *fun2(void *arg)
{
    int y;
    sem_wait(&s);

    y = shared;
    printf("Thread2 reads: %d\n", y);
    y--;
    sleep(1);
    shared = y;
    printf("Thread2 updates shared to %d\n", shared);

    sem_post(&s);
    return NULL;
}
