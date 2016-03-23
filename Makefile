all: ina219 main
	gcc main.o ina219.o -lm -o prog 

ina219: ina219.c ina219.h
	gcc ina219.c -lm -o ina219.o -c 

main: main.c
	gcc main.c -o main.o -c 

clean:
	rm *.o