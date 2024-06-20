#include "control.h"
#include "vision.h"

#define BASE_PRIORITY       80
#define CONTROL_PRIORITY    BASE_PRIORITY + 1
#define VISION_PRIORITY     BASE_PRIORITY + 0

int main() {

    pthread_t controlThread = control_create_thread(CONTROL_PRIORITY);
    pthread_t visionThread = vision_create_thread(VISION_PRIORITY);

    pthread_join(controlThread, NULL);
    pthread_join(visionThread, NULL);

    return 0;
}
