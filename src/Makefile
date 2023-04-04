# Default values for frame store size and variable store size
framesize = 18
varmemsize = 10

# Compile the source files and link them into an executable
mysh: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	gcc -D FRAME_STORE_SIZE=$(framesize) -D VAR_STORE_SIZE=$(varmemsize) -c shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o ready_queue.o scheduler.o disk.o

# Clean up object files and executable
clean:
	rm -f mysh
	rm -f *.o

# Debugging build
debug: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	gcc -D FRAME_STORE_SIZE=$(framesize) -D VAR_STORE_SIZE=$(varmemsize) -c -g shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o ready_queue.o scheduler.o disk.o

# Build and run the shell
run: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make mysh framesize=$(framesize) varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc2.txt
