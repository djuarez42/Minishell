CC=gcc
CFLAGS=-I/usr/include/check
LDFLAGS=-lcheck
SOURCES=test_minishell.c
EXECUTABLE=test_minishell

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) *.o