#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#define STDOUT_PREFIX "cont: "
#define STDERR_PREFIX "cont: "

#define QUEUE_CONT_DATA_NAME "/DRC-CONT-DATA"
#define QUEUE_CONT_DATA_ELMS 10
#define QUEUE_CONT_DATA_SIZE 64

#define SENDER_UNKN "unknown"
#define SENDER_VISN "visn"
#define SENDER_USND "usnd"

#define GET_NTH_INT(buffer, n)  (((int *) buffer)[n])
#define GET_SENDER(message)     GET_NTH_INT(message, 0)
#define GET_X(message)          GET_NTH_INT(message, 1)
#define GET_Y(message)          GET_NTH_INT(message, 2)
#define GET_DIST_IN_FRONT(message)  GET_NTH_INT(message, 1)

#define NSEC_PER_SEC 1000000000

struct SenseData {
    int x;
    int y;
    int distInFront;
};

enum Sender {
    SD_NONE = 0,
    SD_VISN,
    SD_USND
};

int update_sense_data(
        struct SenseData *sdata,
        mqd_t queue,
        const struct timespec *timeout);
char *get_sender_str(enum Sender s);

int main(int argc, char **argv) {
    struct mq_attr attr;
    attr.mq_maxmsg = QUEUE_CONT_DATA_ELMS; 
    attr.mq_msgsize = QUEUE_CONT_DATA_SIZE; 

    mq_unlink(QUEUE_CONT_DATA_NAME);

    mqd_t queue = mq_open(
        QUEUE_CONT_DATA_NAME, 
        O_CREAT | O_RDONLY,
        S_IRWXU,
        &attr);

    if (queue == (mqd_t) -1) {
        perror(STDERR_PREFIX "mq_open failed");
        exit(EXIT_FAILURE);
    }

    printf(STDOUT_PREFIX "Opened queue \"%s\"\n", QUEUE_CONT_DATA_NAME);


    struct SenseData senseData;

    int sender = SD_NONE;
 
    struct timespec *timeout;
    struct timespec maxTime;
    maxTime.tv_sec = 100L * 365L * 24L * 60L * 60L;
    maxTime.tv_nsec = 999999999L;

    while (1) {
        // On first pass, block until a message is received, then remove
        // messages from the queue until it is empty.
        sender = SD_VISN;
        timeout = &maxTime;
        while (sender != SD_NONE) {
            sender = update_sense_data(&senseData, queue, timeout);
            timeout = NULL;

            if (sender == SD_NONE) {
                break;
            }

            printf(
                STDOUT_PREFIX 
                "Received message from %s\n",
                get_sender_str(sender));
        }

        printf(
            STDOUT_PREFIX 
            "Sense data (%i, %i, %i)\n",
            senseData.x,
            senseData.y,
            senseData.distInFront);

        struct timespec delay;
        delay.tv_sec = 5;
        delay.tv_nsec = 0;
        nanosleep(&delay, NULL);
    }

}

static void timespec_sum(struct timespec *dest, const struct timespec *toAdd) {
    if (toAdd == NULL || dest == NULL) {
        return;
    }

    dest->tv_sec += toAdd->tv_sec;
    dest->tv_nsec += toAdd->tv_nsec;

    while (dest->tv_nsec >= NSEC_PER_SEC) {
        dest->tv_sec++;
        dest->tv_nsec -= NSEC_PER_SEC;
    }
}

int update_sense_data(
        struct SenseData *sdata,
        mqd_t queue,
        const struct timespec *timeout) {
    size_t messageLen = 0;
    char message[QUEUE_CONT_DATA_SIZE];

    struct timespec absTimeout;
    clock_gettime(CLOCK_REALTIME, &absTimeout); 
    timespec_sum(&absTimeout, timeout);

    messageLen = mq_timedreceive(
        queue,
        message,
        QUEUE_CONT_DATA_SIZE,
        NULL,
        &absTimeout);

    if (messageLen == -1) {
        if (errno == ETIMEDOUT) {
            return SD_NONE;
        }

        perror(STDERR_PREFIX "mq_receive failed");
        exit(EXIT_FAILURE);
    }

    enum Sender sender = GET_SENDER(message);
    switch(sender) {
        default:
            break;
        case SD_VISN:
            sdata->x = GET_X(message);
            sdata->y = GET_Y(message);
            break;
        case SD_USND:
            sdata->distInFront = GET_DIST_IN_FRONT(message);
            break;
    }

    return sender;
}

char *get_sender_str(enum Sender s) {
    switch(s) {
        default:
            return SENDER_UNKN;
        case SD_VISN:
            return SENDER_VISN;
        case SD_USND:
            return SENDER_USND;
    }
}

