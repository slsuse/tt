all:	test tt

DEBUG=-g3
CDIALECT=gnu99

test:	test.o tt.o ttf.o tt.h ttf.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} test.o tt.o ttf.o -o probe

tt:	ttui.o ttf.o tt.o ttf.h tt.h Makefile
	gcc -O0 ${DEBUG} -Wall -std=${CDIALECT} -o tt ttui.o tt.o ttf.o

ttui.o:	ttui.c ttf.h tt.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} -c ttui.c


test.o:	test.c tt.h ttf.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} -c test.c

tt.o:	tt.c tt.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} -c tt.c

ttf.o:	ttf.c ttf.h tt.h Makefile
	gcc -O0 -g3 -Wall -std=${CDIALECT} -c ttf.c

clean:
	rm *.o
