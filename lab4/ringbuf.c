/*
 * Producer/consumer program using a ring buffer (lab 5).
 *
 *
 * Team members: Alspencer Omondi and Bryce Mey
 *
 *      Student 1 <aomondi@cs.hmc.edu>
 *      Student 2 <bmey@cs.hmc.edu>
 */

#include <pthread.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]);
void *producer(void *arg);
void *consumer(void *arg);
void thread_sleep(unsigned int ms);

#define BUFSLOTS 10

/*
 * Structure used to hold messages between producer and consumer.
 */
struct message
{
    int value;          /* Value to be passed to consumer */
    int consumer_sleep; /* Time (in ms) for consumer to sleep */
    int line;           /* Line number in input file */
    int print_code;     /* Output code; see below */
    int quit;           /* Nonzero if consumer should exit ("value" ignored) */
};

/*
 * The ring buffer itself.
 */
static struct message buffer[BUFSLOTS];

    int bufferLength = 10;  /* Stores the buffer length*/
    int readIndex = 0;      /* Stores the buffer length*/
    int writeIndex = 0;     /* Stores the buffer length*/

/*
 * NEEDSWORK
 *
 * Sample conditions and mutexes.  You will need to customize these.
 */

static pthread_cond_t sample_condition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
    pthread_t consumer_tid;

    /*
     * Make sure output appears right away.
     */
    setlinebuf(stdout);

    /*
     * Create a thread for the consumer.
     */
    if (pthread_create(&consumer_tid, NULL, consumer, NULL) != 0)
    {
        fprintf(stderr, "Couldn't create consumer thread\n");
        return 1;
    }

    /*
     * We will call the producer directly.  (Alternatively, we could
     * spawn a thread for the producer, but then we would have to join
     * it.)
     */
    producer(NULL);

    /*
     * The producer has terminated.  Clean up the consumer, which might
     * not have terminated yet.
     */
    if (pthread_join(consumer_tid, NULL) != 0)
    {
        fprintf(stderr, "Couldn't join with consumer thread\n");
        return 1;
    }
    return 0;
}

void *producer(void *arg)
{
    unsigned int consumer_sleep; /* Space for reading in data */
    int line = 0;                /* Line number in input */
    int print_code;              /* Space for reading in data */
    unsigned int producer_sleep; /* Space for reading in data */
    int value;                   /* Space for reading in data */

    while (scanf("%d%u%u%d",
                 &value, &producer_sleep, &consumer_sleep, &print_code) == 4)
    {
        line++;
        thread_sleep(producer_sleep);

        // Message to the consumer in the buffer

        // %d%u%u%d

        /*
         * After sending values to the consumer, print what was asked.
         */
        if (print_code == 1 || print_code == 3)
            printf("Produced %d from input line %d\n", value, line);
    }
    // terminate the consumer

    return NULL;
}

void *consumer(void *arg)
{
    /*
     * NEEDSWORK
     *
     * Write the consumer here.
     */

    return NULL;

    /*
     * After sending values to the consumer, print what was asked.
     */
    // if (print_code == 1  ||  print_code == 3)
    //     printf(  "Final sum is %d\n", value, line);
}

void thread_sleep(unsigned int ms)
{
    struct timespec sleep_time;

    if (ms == 0)
        return;

    /*
     * NEEDSWORK
     *
     * These assignment statements are dummies.  You will need to write
     * correct values.  Remember that tv_nsec cannot exceed one billion!
     */
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 250000000;
    nanosleep(&sleep_time, NULL);
}
