# makefile2.0
# Voditi racuna da nema blanko znakova nakon definisanja putanja
IDIR_ASS = inc/assembler
SRCDIR_ASS = src/assembler

IDIR_LINKER = inc/assembler
SRCDIR_LINKER = src/linker

IDIR_EMULATOR = inc/emulator
SRCDIR_EMULATOR = src/emulator

BINDIR = bin
OBJDIR = obj
CC = g++

CFLAGS_ASS = -w -I$(IDIR_ASS)
CFLAGS_LINKER = -w -I$(IDIR_LINKER)
CFLAGS_EMULATOR = -w -I$(IDIR_EMULATOR)

ASSEMBLER = assembler
LINKER = linker
EMULATOR = emulator

SRC_ASS = $(wildcard $(SRCDIR_ASS)/*.cpp)

all: $(ASSEMBLER) $(LINKER) $(EMULATOR)

$(ASSEMBLER): $(SRCDIR_ASS)/*.cpp
	$(CC) -o $@ $^
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(LINKER): $(SRCDIR_LINKER)/*.cpp
	$(CC) -o $@ $^

$(EMULATOR): $(SRCDIR_EMULATOR)/*.cpp
	$(CC) -o $@ $^

debug: CC += -g
debug: $(ASSEMBLER) $(LINKER) $(EMULATOR)

clean:
	rm -f $(ASSEMBLER)
	rm -f $(LINKER)
	rm -f $(EMULATOR)
	rm -r $(OBJDIR)
	rm -r $(BINDIR)
.PHONY: clean