#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "vision.h"

#define VISION_PRIORITY 80

int main() {

    // lock memory to avoid page faults.
    // See: https://shuhaowu.com/blog/2022/03-linux-rt-appdev-part3.html#virtual-memory-avoid-page-faults-and-use-mlockall
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        fprintf(stderr, "mlockall failed\n");
        exit(1);
    }

    int error;
    pthread_t visionThread = vision_create_thread(VISION_PRIORITY, &error);
    if (error) {
        return error;
    }

    pthread_join(visionThread, NULL);

    return 0;
}
