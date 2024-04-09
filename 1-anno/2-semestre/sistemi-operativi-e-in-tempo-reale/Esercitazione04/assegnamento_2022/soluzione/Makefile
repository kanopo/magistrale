EXECUTABLES=server hub_campo sensore lettore

all: $(EXECUTABLES) 

server: server.o list.o util.o
	gcc -Wall -o $@ $^
	
hub_campo: hub_campo.o list.o util.o
	gcc -Wall -o $@ $^
	
sensore: sensore.o list.o util.o
	gcc -Wall -o $@ $^
	
lettore: lettore.o list.o util.o
	gcc -Wall -o $@ $^
	
%.o: %.c
	gcc -Wall -c $<
	
clean:
	rm -rf $(EXECUTABLES) *.o *~
