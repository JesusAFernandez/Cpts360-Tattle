tattle: main.o tattle.c
	gcc -g -Wall -Wstrict-prototypes main.c tattle.c -o tattle
main.o: main.c tattle.h
	gcc -g -Wall -Wstrict-prototypes -c main.c
tattle.o: tattle.c tattle.h
	gcc -g -Wall -Wstrict-prototypes -c tattle.c
clean: 
	rm *.o tattle