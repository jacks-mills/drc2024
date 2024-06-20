CC := g++
CPPFLAGS := -Wall -pedantic

SRCS := control.cc vision.h\
	vision.cc vision.h \
	main.cc
TARG := robot

OBJS := $(patsubst %.cc, %.o, $(filter %.cc, $(SRCS)))

.PHONY: all clean
    
all: $(TARG)
    
clean:
	rm -f $(TARG) $(OBJS)
    
$(TARG): $(OBJS)
	$(CC) $(CPPFLAGS)   -o $@ $^
