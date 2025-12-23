#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

#define PARKING_SPOTS 2
#define THREADS 5

sem_t parking;
int spots[PARKING_SPOTS] = {0, 0};  // 2 resource for parking spot
                                    // 0: free, 1: parked
void *car(void *arg)
{
    int id = *(int *)arg;

    sem_wait(&parking);

    int spot = -1;
    for (int i = 0; i < PARKING_SPOTS; i++)
    {
        if (spots[i] == 0)
        {
            spots[i] = 1;
            spot = i;
            break;
        }
    }

    printf("Car %d parked at spot %d\n", id, spot);
    sleep(1);

    spots[spot] = 0;
    printf("Car %d left spot %d\n", id, spot);

    sem_post(&parking);
    return NULL;
}

int main()
{
    pthread_t t[THREADS];
    int id[THREADS];

    sem_init(&parking, 0, PARKING_SPOTS);

    for (int i = 0; i < THREADS; i++)
    {
        id[i] = i + 1;
        pthread_create(&t[i], NULL, car, &id[i]);
    }

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], NULL);

    return 0;
}
