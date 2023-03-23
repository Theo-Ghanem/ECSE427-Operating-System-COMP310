mysh: shell.c interpreter.c shellmemory.c
	gcc -c shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c paging.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o ready_queue.o scheduler.o paging.o

clean: 
	rm mysh; rm *.o

debug: shell.c interpreter.c shellmemory.c
	gcc -c -g shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c paging.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o ready_queue.o scheduler.o paging.0
