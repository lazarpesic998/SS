# makefile2.0
# Voditi racuna da nema blanko znakova nakon definisanja putanja
IDIR = inc
SRCDIR = src
BINDIR = bin
CC = g++
CCFLAGS = -g
CFLAGS = -w -I$(IDIR)
PROGRAM = assembler
SRC = $(wildcard $(SRCDIR)/*.cpp)

$(PROGRAM): $(SRCDIR)/*.cpp
	$(CC) -o $@ $^
	mkdir $(BINDIR)

	

clean:
	rm -f $(PROGRAM)
	rm -r $(BINDIR)
.PHONY: clean