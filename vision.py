from posix_ipc import MessageQueue
import struct
from time import sleep

QUEUE_CONT_VISN_DATA_NAME = "/DRC-CONT-VISN-DATA"
QUEUE_CONT_VISN_DATA_ELMS = 10
QUEUE_CONT_VISN_DATA_SIZE = 64

STDOUT_PREFIX = "vision: "
STDOUT_PREFIX = "vision: "

def main():
    q = MessageQueue(
        QUEUE_CONT_VISN_DATA_NAME,
        flags = O_CREAT,
        mode = 0o700,
        max_messages = QUEUE_CONT_VISN_DATA_ELMS,
        max_message_size = QUEUE_CONT_VISN_DATA_SIZE,
        read = False,
        write = True)

    count = 0
    while True:
        count = count + 1
        sleep(2)

if (__name__ == "__main__"):
    main()
