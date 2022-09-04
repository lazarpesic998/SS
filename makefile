# makefile2.0
# Voditi racuna da nema blanko znakova nakon definisanja putanja
IDIR_ASS = inc/assembler
SRCDIR_ASS = src/assembler

IDIR_LINKER = inc/assembler
SRCDIR_LINKER = src/linker

BINDIR = bin
OBJDIR = obj
CC = g++

CFLAGS_ASS = -w -I$(IDIR_ASS)
CFLAGS_LINKER = -w -I$(IDIR_LINKER)

ASSEMBLER = assembler
LINKER = linker

SRC_ASS = $(wildcard $(SRCDIR_ASS)/*.cpp)

all: $(ASSEMBLER) $(LINKER)

$(ASSEMBLER): $(SRCDIR_ASS)/*.cpp
	$(CC) -o $@ $^
	mkdir $(OBJDIR)
	mkdir $(BINDIR)
$(LINKER): $(SRCDIR_LINKER)/*.cpp
	$(CC) -o $@ $^

debug: CC += -g
debug: $(ASSEMBLER) $(LINKER)

clean:
	rm -f $(ASSEMBLER)
	rm -f $(LINKER)
	rm -r $(OBJDIR)
	rm -r $(BINDIR)
.PHONY: clean