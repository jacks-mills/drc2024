#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

#define STDOUT_PREFIX "cont: "
#define STDERR_PREFIX "cont: "

#define QUEUE_CONT_DATA_NAME "/DRC-CONT-DATA"
#define QUEUE_CONT_DATA_ELMS 10
#define QUEUE_CONT_DATA_SIZE 64

#define SNDR_NONE 0
#define SNDR_VISN 1
#define SNDR_USND 2

#define SNDR_UNKN_STR "unknown"
#define SNDR_VISN_STR "visn"
#define SNDR_USND_STR "usnd"

#define GET_NTH_INT(buffer, n)      (((int *) buffer)[n])
#define GET_SENDER(message)         GET_NTH_INT(message, 0)
#define GET_LANE_CENT_X(message)    GET_NTH_INT(message, 1)
#define GET_LANE_CENT_Y(message)    GET_NTH_INT(message, 2)
#define GET_DIST_IN_FRONT(message)  GET_NTH_INT(message, 1)

#define NSEC_PER_SEC 1000000000

struct State {
    int laneCentreX;
    int laneCentreY;
    int distInFront;
};


void unlink_data_queue();
void attach_unlink_sig_handler();
mqd_t open_queue();
void update_sense_data(struct State *state, mqd_t dataQueue);


int main(int argc, char **argv) {
    atexit(unlink_data_queue);
    attach_unlink_sig_handler();

    mqd_t dataQueue = open_queue();
    printf(STDOUT_PREFIX "Opened queue \"%s\"\n", QUEUE_CONT_DATA_NAME);

    struct State state;
    memset(&state, 0, sizeof(state));
    while (1) {
        update_sense_data(&state, dataQueue);

        printf(
            STDOUT_PREFIX
            "Sense data (%i, %i, %i)\n",
            state.laneCentreX,
            state.laneCentreY,
            state.distInFront);

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

void unlink_data_queue() {
    mq_unlink(QUEUE_CONT_DATA_NAME);
    printf(STDOUT_PREFIX "Unlinked queue \"%s\"\n", QUEUE_CONT_DATA_NAME);
}

void sig_handler_unlink_then_exit(int signum) {
    printf(STDOUT_PREFIX "Received signal %i\n", signum);
    exit(signum);
}

void attach_unlink_sig_handler() {
    struct sigaction sa;

    sa.sa_handler = sig_handler_unlink_then_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    int signals[] = {SIGINT, SIGTERM, SIGABRT, SIGQUIT, SIGHUP, 0};

    for (int *cursor = signals; *cursor != 0; cursor++) {
        int error = sigaction(*cursor, &sa, NULL);
        if (error == -1) {
            perror(STDERR_PREFIX "sigaction failed");
            exit(EXIT_FAILURE);
        }
    }
}

mqd_t open_queue() {
    mqd_t queue;

    struct mq_attr attr;
    attr.mq_maxmsg = QUEUE_CONT_DATA_ELMS;
    attr.mq_msgsize = QUEUE_CONT_DATA_SIZE;

    queue = mq_open(
        QUEUE_CONT_DATA_NAME,
        O_CREAT | O_RDONLY,
        S_IRWXU,
        &attr);

    if (queue == (mqd_t) -1) {
        perror(STDERR_PREFIX "mq_open failed");
        exit(EXIT_FAILURE);
    }

    return queue;
}

int extract_data(struct State *state, char *message) {
    int sender = GET_SENDER(message);
    switch(sender) {
        default:
            sender = SNDR_NONE;
            break;
        case SNDR_VISN:
            state->laneCentreX = GET_LANE_CENT_X(message);
            state->laneCentreY = GET_LANE_CENT_Y(message);
            break;
        case SNDR_USND:
            state->distInFront = GET_DIST_IN_FRONT(message);
            break;
    }
    return sender;
}

int read_message(char *message, mqd_t queue, const struct timespec *timeout) {
    int bytesRead = 0;

    struct timespec absTimeout;
    clock_gettime(CLOCK_REALTIME, &absTimeout);
    timespec_sum(&absTimeout, timeout);

    bytesRead = mq_timedreceive(
        queue,
        message,
        QUEUE_CONT_DATA_SIZE,
        NULL,
        &absTimeout);

    if (bytesRead == -1) {
        if (errno == ETIMEDOUT) {
            return 0;
        }

        perror(STDERR_PREFIX "mq_receive failed");
        exit(EXIT_FAILURE);
    }

    return bytesRead;
}

char *get_sender_str(int s) {
    switch(s) {
        default:
            return SNDR_UNKN_STR;
        case SNDR_VISN:
            return SNDR_VISN_STR;
        case SNDR_USND:
            return SNDR_USND_STR;
    }
}

void update_sense_data(struct State *state, mqd_t dataQueue) {
    int sender;
    int bytesRead = 0;
    char message[QUEUE_CONT_DATA_SIZE];

    // 100 year timeout
    struct timespec maxTimeout;
    maxTimeout.tv_sec = 100L * 365L * 24L * 60L * 60L;
    maxTimeout.tv_nsec = 999999999L;

    bytesRead = read_message(message, dataQueue, &maxTimeout);
    while (bytesRead > 0) {
        sender = extract_data(state, message);
        printf(
            STDOUT_PREFIX
            "Received message from %s\n",
            get_sender_str(sender));

        bytesRead = read_message(message, dataQueue, NULL);
    }
}


