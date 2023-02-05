mysh: shell.c interpreter.c shellmemory.c
	gcc -c shell.c interpreter.c shellmemory.c
	gcc -o mysh shell.o interpreter.o shellmemory.o

clean: 
	rm mysh; rm *.o

debug: shell.c interpreter.c shellmemory.c
	gcc -c -g shell.c interpreter.c shellmemory.c
	gcc -o mysh shell.o interpreter.o shellmemory.o
