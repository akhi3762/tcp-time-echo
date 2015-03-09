#This is a sample Makefile which compiles source files named:
# - tcpechotimeserv.c
# - tcpechotimecli.c
# - time_cli.c
# - echo_cli.c
# and creating executables: "server", "client", "time_cli"
# and "echo_cli", respectively.
#
# It uses various standard libraries, and the copy of Stevens'
# library "libunp.a" in ~cse533/Stevens/unpv13e_solaris2.10 .
#
# It also picks up the thread-safe version of "readline.c"
# from Stevens' directory "threads" and uses it when building
# the executable "server".
#
# It is set up, for illustrative purposes, to enable you to use
# the Stevens code in the ~cse533/Stevens/unpv13e_solaris2.10/lib
# subdirectory (where, for example, the file "unp.h" is located)
# without your needing to maintain your own, local copies of that
# code, and without your needing to include such code in the
# submissions of your assignments.
#
# Modify it as needed, and include it with your submission.

CC = gcc

LIBS = -lresolv -lsocket -lnsl -lpthread\
	/home/courses/cse533/Stevens/unpv13e_solaris2.10/libunp.a\
	
FLAGS = -g -O2

CFLAGS = ${FLAGS} -I/home/courses/cse533/Stevens/unpv13e_solaris2.10/lib

all: client server echoclient timeclient
#echo_cli time_cli 


timeclient: timeclient.o
	${CC} ${FLAGS} -o timeclient timeclient.o ${LIBS}
timeclient.o: timeclient.c
	${CC} ${CFLAGS} -c timeclient.c


echoclient: echoclient.o
	${CC} ${FLAGS} -o echoclient echoclient.o ${LIBS}
echoclient.o: echoclient.c
	${CC} ${CFLAGS} -c echoclient.c


# server uses the thread-safe version of readline.c

server: daytimetcpsrv.o readline.o
	${CC} ${FLAGS} -o server daytimetcpsrv.o readline.o ${LIBS}
tcpechotimesrv.o: daytimetcpsrv.c
	${CC} ${CFLAGS} -c daytimetcpsrv.c


client: daytimetcpcli.o
	${CC} ${FLAGS} -o client daytimetcpcli.o ${LIBS}
tcpechotimecli.o: daytimetcpcli.c
	${CC} ${CFLAGS} -c daytimetcpcli.c


# pick up the thread-safe version of readline.c from directory "threads"

readline.o: /home/courses/cse533/Stevens/unpv13e_solaris2.10/threads/readline.c
	${CC} ${CFLAGS} -c /home/courses/cse533/Stevens/unpv13e_solaris2.10/threads/readline.c


clean:
	rm  server daytimetcpsrv.o client daytimetcpcli.o readline.o echoclient.o timeclient.o timeclient echoclient


