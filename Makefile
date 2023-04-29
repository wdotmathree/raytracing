CC=gcc
CFLAGS=-fno-plt -fno-pic -static -static-libgcc -Wall -Wextra -fno-trapping-math -fno-math-errno -fno-signed-zeros -march=native -lm -g
SRCS=$(wildcard *.c)
HDRS=$(wildcard *.h)
OBJS=$(SRCS:.c=.o)
CUS=$(wildcard *.cu)
CUOBJS=$(CUS:.cu=.o)

.PHONY: debug release test clean

debug: a.out

release: CFLAGS += -O3
release: a.out
	strip -s a.out

a.out: $(OBJS) $(CUOBJS)
	$(CC) $(OBJS) $(CUOBJS) $(CFLAGS)

%.o: %.c $(HDRS) Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.cu $(HDRS) Makefile
	nvcc -c $< -o $@

clean:
	rm -f $(OBJS) a.out
