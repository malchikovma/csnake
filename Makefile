CC=gcc
CFLAGS=-Wall

build: snake-cli

test: snake-test
	./snake-test
clean:
	rm -f snake-test snake-cli


snake-test: snake_test.c snake.c
	$(CC) $(CFLAGS) -o $@ $< -lm

snake-cli: snake_cli.c snake.c
	$(CC) $(CFLAGS) -o $@ $< -lm
