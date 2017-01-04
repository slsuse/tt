tt.o:	tt.c tt.h Makefile
	gcc -Wall -std=c99 -c tt.c

clean:
	rm *.o

