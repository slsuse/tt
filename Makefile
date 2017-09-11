all:	test ttp

DEBUG=-g3
CDIALECT=gnu99

test:	test.o tt.o ttf.o tt.h ttf.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} test.o tt.o ttf.o -o probe

ttp:	ttp.o ttf.o tt.o ttf.h tt.h Makefile
	gcc -O0 ${DEBUG} -Wall -std=${CDIALECT} -o ttp ttp.o tt.o ttf.o

ttp.o:	ttp.c ttf.h tt.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} -c ttp.c


test.o:	test.c tt.h ttf.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} -c test.c

tt.o:	tt.c tt.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} -c tt.c

ttf.o:	ttf.c ttf.h tt.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} -c ttf.c

clean:
	rm *.o
