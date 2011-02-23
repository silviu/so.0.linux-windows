CC = cl
CFLAGS= 
PGR = tema0

build: hash.c run_hash.c
	$(CC) $(CFLAGS) /Fe$(PGR).exe hash.c run_hash.c

clean:
	del $(PGR).exe *.obj

