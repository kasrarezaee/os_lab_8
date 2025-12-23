#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_COUNT 5

int buffer = 0;

void *reader(void *arg)
{
    int id = *(int *)arg;

    while (buffer < MAX_COUNT)
    {
        printf("Reader %d reads count = %d\n", id, buffer);
        sleep(1);
    }
    return NULL;
}

void *writer(void *arg)
{
    while (buffer < MAX_COUNT)
    {
        buffer++;
        printf("Writer (PID=%ld) writes count = %d\n",
               (long)pthread_self(), buffer);
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t r1, r2, w;
    int id1 = 1, id2 = 2;

    pthread_create(&r1, NULL, reader, &id1);
    pthread_create(&r2, NULL, reader, &id2);
    pthread_create(&w, NULL, writer, NULL);

    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w, NULL);

    return 0;
}
