all:	test

test:	test.o tt.o ttf.o tt.h ttf.h Makefile
	gcc -O0 -g3 -Wall -std=c99 test.o tt.o ttf.o

test.o:	test.c tt.h ttf.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -c test.c

tt.o:	tt.c tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -c tt.c

ttf.o:	ttf.c ttf.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -c ttf.c

clean:
	rm *.o

