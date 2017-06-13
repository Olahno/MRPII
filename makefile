all:
	gcc -o mrpii mrpiimain.c MRPII.c -Wall `pkg-config --cflags --libs gtk+-3.0` -rdynamic -lwiringPi -lcrypt
