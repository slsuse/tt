all:	test ttp

test:	test.o tt.o tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -o probe test.o tt.o

ttp:	ttp.o tt.o tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -o ttp ttp.o tt.o

ttp.o:	ttp.c tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -c ttp.c

test.o:	test.c tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -c test.c

tt.o:	tt.c tt.h Makefile
	gcc -O0 -g3 -Wall -std=c99 -c tt.c

clean:
	rm *.o
