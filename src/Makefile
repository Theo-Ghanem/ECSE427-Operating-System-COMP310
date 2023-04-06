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
run1: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=$(framesize) varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc1.txt
run2: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=$(framesize) varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc2.txt
run3: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=21 varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc3.txt
run4: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=$(framesize) varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc4.txt
run5: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=6 varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc5.txt
run6: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=$(framesize) varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc6.txt
run7: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=$(framesize) varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc7.txt
run8: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=21 varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc8.txt
run9: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=$(framesize) varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc9.txt
run10: shell.c interpreter.c shellmemory.c pcb.c ready_queue.c scheduler.c disk.c
	make clean
	make debug framesize=6 varmemsize=$(varmemsize)
	./mysh < ../tc/a3/T_tc10.txt



