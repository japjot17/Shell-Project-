all: cshell.c
	gcc -g -o cshell cshell.c 
clean:
	rm -rf *.o cshell 
