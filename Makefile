# Universidade Federal do Rio de Janeiro
# Escola Politecnica
# Engenharia de Computação de Informação
# Prof. Daniel R. Figueiredo, Ricardo Marroquim
# Teoria dos Grafos - Turma 2013/2
# Author: Vinícius Ferrão
# Description: Default Makefile

#CC = gcc-mp-4.8
#LD = gcc-mp-4.8
CC = clang
LD = clang

#CFLAGS = -Wall -pipe -c
CFLAGS = -Wall -O4 -march=native -pipe -c
DFLAGS = -g -D_DEBUG_
LFLAGS = -Wall -o

# Executables
EXECS = main

# Objects
OBJMAIN = main.o functions.o
#OBJALT = alt.o functions.o

# Librarys
#LIBNCURSES = -lncurses
#LIBCRYPT = -lcrypt

# Rules
.c.o:
	$(CC) $(CFLAGS) $(DFLAGS) $<

all: $(EXECS)

main: $(OBJMAIN)
	$(LD) $(LFLAGS) $@ $(OBJMAIN) 

#alt: $(OBJALT)
#	$(LD) $(LFLAGS) $@ $(OBJALT)

clean:
	rm -f *.o
	rm -f *.c~
	rm -f *.h~
	rm -f *.html~
	rm -f *.exe*

