all:	test

test:	test.o tt.o tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 test.o tt.o

test.o:	test.c tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -c test.c

tt.o:	tt.c tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -c tt.c

clean:
	rm *.o

