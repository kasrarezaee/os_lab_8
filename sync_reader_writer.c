#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_COUNT 5

int buffer = 0;
int read_count = 0;

sem_t mutex;
sem_t wrt;

void *reader(void *arg)
{
    int id = *(int *)arg;

    while (1)
    {
        sem_wait(&mutex);
        read_count++;
        if (read_count == 1)
            sem_wait(&wrt);   // first reader locks wrt
        sem_post(&mutex);

        printf("Reader %d reads count = %d\n", id, buffer);
        sleep(1);

        sem_wait(&mutex);
        read_count--;
        if (read_count == 0)
            sem_post(&wrt);
        sem_post(&mutex);

        sleep(1);
        if (buffer >= MAX_COUNT)
            break;
    }
    return NULL;
}

void *writer(void *arg)
{
    while (1)
    {
        sem_wait(&wrt);

        if (buffer >= MAX_COUNT)
        {
            sem_post(&wrt);
            break;
        }

        buffer++;
        printf("Writer (PID=%ld) writes count = %d\n",
               (long)pthread_self(), buffer);

        sem_post(&wrt);
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t r1, r2, w;
    int id1 = 1, id2 = 2;

    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    pthread_create(&w, NULL, writer, NULL);
    sleep(1); // let writer increment buffer
    pthread_create(&r1, NULL, reader, &id1);
    pthread_create(&r2, NULL, reader, &id2);

    sleep(1);

    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w, NULL);

    return 0;
}
