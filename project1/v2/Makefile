TARGETS = mc2
OBJS = mc2.o LinkedList.o
SOURCES = mc2.c LinkedList.c
CFLAGS = -Wall

all: mc2

mc2: $(OBJS)
	gcc $(CFLAGS) $(OBJS) -o $(TARGETS)

mc2.o: mc2.c
	gcc $(CFLAGS) -c mc2.c

LinkedList.o: LinkedList.c LinkedList.h
	gcc $(CFLAGS) -c LinkedList.c

clean:
	rm -f $(TARGETS) $(OBJS)
