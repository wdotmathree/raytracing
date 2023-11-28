CC=nvcc
CFLAGS=-rdc=true -lm -lpthread -g
SRCS=$(wildcard *.cu)
HDRS=$(wildcard *.hpp)
OBJS=$(SRCS:.cu=.o)

.PHONY: debug release test clean

debug: a.out

release: CFLAGS += -O3
release: a.out
	# strip -s a.out

a.out: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS)

%.o: %.cu $(HDRS) Makefile
	nvcc -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) a.out
