all: ina219 main
	gcc -lm -o prog main.o ina219.o

ina219: ina219.c ina219.h
	gcc -lm -o ina219.o -c ina219.c

main: main.c
	gcc -o main.o -c main.c

clean:
	rm *.o