CFLAGS = -Wno-implicit-function-declaration 

all: final

shell.o: shell.c
	gcc $(CFLAGS) -c shell.c

final: shell.o
	gcc $(CFLAGS) shell.o -o final

clean:
	rm shell.o final shconfig.txt