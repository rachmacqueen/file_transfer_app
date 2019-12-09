# make all
all : fileServer.o fileClient.o IO.o logger.o config.o networking.o authentication.o signals.o fileServer_program fileClient_program


# targets to compile each file
fileServer.o: fileServer.c IO.h logger.h config.h networking.h authentication.h signals.h
	gcc -c -Wall -Werror fileServer.c

fileClient.o: fileClient.c IO.h logger.h config.h networking.h authentication.h signals.h
	gcc -c -Wall -Werror fileClient.c

IO.o: IO.c
	gcc -c -Wall -Werror IO.c

logger.o: logger.c
	gcc -c -Wall -Werror logger.c

config.o: config.c
	gcc -c -Wall -Werror config.c

networking.o: networking.c
	gcc -c -Wall -Werror networking.c

authentication.o: authentication.c
	gcc -c -Wall -Werror authentication.c

signals.o: signals.c
	gcc -c -Wall -Werror signals.c


# target to link the whole program 
fileServer_program: fileServer.o IO.o logger.o config.o networking.o authentication.o signals.o
	gcc fileServer.o IO.o logger.o config.o networking.o authentication.o signals.o -o fileServer_program -lpthread

fileClient_program: fileClient.o IO.o logger.o config.o networking.o authentication.o signals.o
	gcc fileClient.o IO.o logger.o config.o networking.o authentication.o signals.o -o fileClient_program

# make clean
clean: 
	rm -rf *.o *-core
