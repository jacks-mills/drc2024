C := gcc
CFLAGS := -Wall -pedantic -std=gnu99 -g

SRCS := main.c \
	control.c control.h \
	vision.c vision.h
OBJS := $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))
TARGET := robot

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS)   -o $@ $^
