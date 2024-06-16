#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>

#include <time.h>


#define ERR_PREFIX "vision: "
#define OUT_PREFIX "vision: "

#define NSEC_PER_SEC 1000000000

#include "vision.h"

static void *vision_thread(void *params);

pthread_t vision_create_thread(int priority, int *error) {
    struct sched_param param;
    pthread_attr_t attr;
    pthread_t thread;

    *error = 0;

    // Initialize pthread attributes (default values)
    *error = pthread_attr_init(&attr);
    if (*error) {
        fprintf(stderr, ERR_PREFIX "init pthread attributes failed\n");
        return 0;
    }

    // Set a specific stack size
    *error = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (*error) {
        fprintf(stderr, ERR_PREFIX "pthread setstacksize failed\n");
        return 0;
    }

    // Set scheduler policy and priority of pthread
    *error = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (*error) {
        fprintf(stderr, ERR_PREFIX "pthread setschedpolicy failed\n");
        return 0;
    }

    param.sched_priority = priority;
    *error = pthread_attr_setschedparam(&attr, &param);

    if (*error) {
        fprintf(stderr, ERR_PREFIX "pthread setschedparam failed\n");
        return 0;
    }

    // Use scheduling parameters of attr
    *error = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (*error) {
        fprintf(stderr, ERR_PREFIX "pthread setinheritsched failed\n");
        return 0;
    }

    // Create a pthread with specified attributes
    *error = pthread_create(&thread, &attr, vision_thread, NULL);
    if (*error) {
        fprintf(stderr, ERR_PREFIX "create pthread failed\n");
        return 0;
    }

    return thread;
}


void timespec_add_ns(struct timespec *ts, time_t nsec) {
    ts->tv_nsec += nsec;
    while (ts->tv_nsec >= NSEC_PER_SEC) {
        ts->tv_nsec -= NSEC_PER_SEC;
        ts->tv_sec++;
    }
}

static void *vision_thread(void *params) {
    struct timespec next_activation;
    clock_gettime(CLOCK_MONOTONIC, &next_activation);

    const time_t delay = NSEC_PER_SEC * 0.5; // 500ms

    timespec_add_ns(&next_activation, delay);

    while (1) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation, NULL);

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        long latency = (now.tv_sec - next_activation.tv_sec) * NSEC_PER_SEC + (now.tv_nsec - next_activation.tv_nsec);
        printf(OUT_PREFIX "Latency: %li ns\n", latency);

        timespec_add_ns(&next_activation, delay);
    }

    return NULL;
}
