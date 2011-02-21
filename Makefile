COMPLILER_LINUX = gcc
COMPILER_WINDOWS = cl

CFLAGS_LINUX = -Wall -g
CFLAGS_WINDOWS = 

PGR = table

linux: hash.c $(PGR).c
	$(COMPLILER_LINUX) $(CFLAGS_LINUX) $(PGR).c -o $(PGR)

win: hash.c $(PGR).c
	$(COMPILER_WINDOWS) $(CFLAGS_WINDOWS) $(PGR).c

clean:
	rm -rf $(PGR) $(PGR).exe *.o *.obj
	
