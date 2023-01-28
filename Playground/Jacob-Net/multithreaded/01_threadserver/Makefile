CC = cc
CFLAGS = -g
BINS=server
OBJS = server.o thread_queue.o


all: $(BINS)

server: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

%: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf *.dSYM $(OBJS) $(BINS)