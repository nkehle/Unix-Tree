CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g 
MAIN = tree
OBJS = tree.o

all : $(MAIN)

$(MAIN): $(OBJS) tree.h
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

tree.o: tree.c tree.h
	$(CC)  $(CFLAGS) -c tree.c

clean :
	rm -f $(OBJS) $(MAIN) core
