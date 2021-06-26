CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic #-ggdb

cvector: main.c vector.c vector.h
	$(CC) $(CFLAGS) -o main main.c vector.c
	./main



