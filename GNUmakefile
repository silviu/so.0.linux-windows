CC = gcc
CFLAGS = -Wall -g -std=c99 -pedantic
PGR = tema0

build: $(PGR)

$(PGR): hash.c run_hash.c
	$(CC) $(CFLAGS) $^ -o $(PGR)

zip: clean
	zip Tema0.zip *
clean:
	rm -rf $(PGR) *.o *.zip

