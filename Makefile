all: qlight.c
	gcc -o qlight qlight.c -Wall -L/lib/i386-linux-gnu/ -L/usr/lib/i386-linux-gnu/ -lusb-1.0 
