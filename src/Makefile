PROG = monitor_IO
OBJS = monitor_IO.o
CC = cc
.PHONY: all
	all: $(PROG)

.SUFFIXES: .o .c
.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROG): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY:clean	
clean:
	rm -f $(PROG) $(OBJS)
