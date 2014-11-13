CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

led-matrixd: led-matrixd-main.o
	$(CC) -o $@ $^ 

led-matrixd-main.o: led-matrixd-main.cpp
	$(CC) -c $^

clean:
	rm *.o
