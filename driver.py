from posix_ipc import *

STDOUT_PREFIX = "driv: "
STDOUT_PREFIX = "driv: "

QUEUE_READ_NAME = "/DRC-DRIV-DATA"
QUEUE_READ_SIZE = 64
QUEUE_READ_ELMS = 10


def main():
    readQueue = open_read_queue(QUEUE_READ_NAME)


def open_read_queue(qName):
    readQueue = MessageQueue(
        qName,
        flags = O_CREAT,
        mode = 0o700,
        max_messages = QUEUE_READ_ELMS,
        max_messaeg_size = QUEUE_READ_SIZE,
        read = True,
        write = False)


if (__name__ == "__main__"):
    main()
