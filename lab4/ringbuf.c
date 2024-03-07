/*
 * Producer/consumer program using a ring buffer (lab 5).
 *
 * Team members: Alspencer Omondi and Bryce Mey
 *
 * Student 1 <aomondi@cs.hmc.edu>
 * Student 2 <bmey@cs.hmc.edu>
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
static pthread_cond_t not_Full = PTHREAD_COND_INITIALIZER;
static pthread_cond_t isFull = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int bufferLength = 10;  /* Stores the buffer length*/
int readIndex = 0;      /* Stores the buffer length*/
int writeIndex = 0;     /* Stores the buffer length*/

/*
 * Main function where the program starts execution.
 */
int main(int argc, char *argv[])
{
    setlinebuf(stdout); // From section 3 in WriteUp

    pthread_t consumer_tid;

    /*
     * Create a thread for the consumer
     */
    if (pthread_create(&consumer_tid, NULL, consumer, NULL) != 0)
    {
        fprintf(stderr, "Couldn't create consumer thread\n");
        return 1;
    }
    setlinebuf(stdout);

    if (pthread_join(consumer_tid, NULL) != 0)
    {
        fprintf(stderr, "Couldn't join consumer thread\n");
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

    pthread_exit(NULL);
}

/*
 * Producer function that produces messages and places them in the buffer.
 */
void *producer(void *arg)
{
    unsigned int consumer_sleep; /* Space for reading in data */
    int line = 0;                /* Line number in input */
    int print_code;              /* Space for reading in data */
    unsigned int producer_sleep; /* Space for reading in data */
    int value;                   /* Space for reading in data */

    while (scanf("%d%u%u%d", &value, &producer_sleep, &consumer_sleep, &print_code) == 4)
    {
        line++;
        thread_sleep(producer_sleep);

        pthread_mutex_lock(&mutex);

        // Wait while the buffer is full
        while ((writeIndex + 1) % bufferLength == readIndex)
        {
            pthread_cond_wait(&not_Full, &mutex);
        }

        // Produce a message and update the write index
        buffer[writeIndex].value = value;
        buffer[writeIndex].consumer_sleep = consumer_sleep;
        buffer[writeIndex].line = line;
        buffer[writeIndex].print_code = print_code;
        buffer[writeIndex].quit = 0;

        writeIndex = (writeIndex + 1) % bufferLength;

        // Signal that the buffer is not empty
        pthread_cond_signal(&isFull);
        pthread_mutex_unlock(&mutex);

        // After sending values to the consumer, print what was asked.
        if (print_code == 1 || print_code == 3)
            printf("Produced %d from input line %d\n", value, line);
    }

    pthread_mutex_lock(&mutex);

    // Wait while the buffer is full
    while ((writeIndex + 1) % bufferLength == readIndex)
    {
        pthread_cond_wait(&not_Full, &mutex);
    }

    // Update the write index and signal that the buffer is not empty
    writeIndex = (writeIndex + 1) % bufferLength;

    pthread_cond_signal(&isFull);
    pthread_mutex_unlock(&mutex);

    // Terminate the consumer
    return NULL;

    pthread_exit(NULL);
}

/*
 * Consumer function that consumes messages from the buffer and processes them.
 */
void *consumer(void *arg)
{
    int sum = 0;

    while (1)
    {
        pthread_mutex_lock(&mutex);

        // Wait while the buffer is empty
        while (readIndex == writeIndex)
        {
            pthread_cond_wait(&isFull, &mutex);
        }

        // Consume a message and update the read index
        int value = buffer[readIndex].value;
        int consumer_sleep = buffer[readIndex].consumer_sleep;
        int line = buffer[readIndex].line;
        int print_code = buffer[readIndex].print_code;
        int quit = buffer[readIndex].quit;

        readIndex = (readIndex + 1) % bufferLength;

        // Signal that the buffer is not full
        pthread_cond_signal(&not_Full);
        pthread_mutex_unlock(&mutex);

        if (quit != 0)
        {
            // Quit signal received, print total and terminate
            printf("Final sum is %d\n", sum);
            return NULL;
        }

        // Sleep for consumer_sleep milliseconds
        thread_sleep(consumer_sleep);

        // Process the value
        sum += value;

        // Print status message if required
        if (print_code == 2 || print_code == 3)
            printf("Consumed %d from input line %d; sum = %d\n", value, line, sum);
    }

    return NULL;
}

/*
 * Function to simulate thread sleep for the given number of milliseconds.
 */
void thread_sleep(unsigned int ms)
{
    struct timespec sleep_time;

    if (ms == 0)
        return;

    sleep_time.tv_sec = ms / 1000;
    sleep_time.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&sleep_time, NULL);
}