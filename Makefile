CC = gcc
CFLAGS = -Wall -O2 -lm
SOURCES = verify_system1.c markov.c
TARGET = verify_system1

all: $(TARGET)

$(TARGET): $(SOURCES) markov.h
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
