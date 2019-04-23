SRCDIR    = src
TSTDIR    = tst
BINDIR    = bin
INCLUDES  = include
REPORTDIR = report

CC = gcc
CFLAGS = -Wall -Wextra -g -fno-stack-protector -z execstack -lpthread -std=gnu11 -I $(INCLUDES)/ -m32

TEX = pdflatex

TSTFILES := $(shell find $(TSTDIR)/ -name "*.h" -o -name "*.c" ! -name "test.c")
SRCFILES := $(shell find $(SRCDIR)/ -name "*.c" ! -name "client.c" ! -name "server.c")
DEPS = $(wildcard $(INCLUDES)/*.h)

.PHONY: test report clean

all: $(BINDIR)/client $(BINDIR)/server $(DEPS)

server: $(BINDIR)/server
client: $(BINDIR)/client
test: $(BINDIR)/test

$(BINDIR)/client: $(SRCDIR)/client.c
	$(CC) $(CFLAGS) $(SRCFILES) $(DEPS) $< -o $@

$(BINDIR)/server: $(SRCDIR)/server.c
	$(CC) $(CFLAGS) $(SRCFILES) $(DEPS) $< -o $@

$(BINDIR)/test: $(TSTDIR)/test.c
	$(CC) $(CFLAGS) $(SRCFILES) $(TSTFILES) $(DEPS) $< -o $@

report: $(REPORTDIR)/report.tex
	$(TEX) -output-directory $(BINDIR) $^

clean:
	rm -f $(wildcard $(BINDIR)/*)
