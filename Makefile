SRCDIR   = src
TSTDIR   = tst
BINDIR   = bin
INCLUDES = include
REPORTDIR= report

CC=gcc
CFLAGS=-Wall -Wextra -g -fno-stack-protector -z execstack -lpthread -std=gnu11 -I $(INCLUDES)/ -m32
DEPS = $(wildcard $(INCLUDES)/%.h)

TEX=pdflatex

all: $(BINDIR)/client $(BINDIR)/server $(DEPS)

$(BINDIR)/client: $(SRCDIR)/client.c
	$(CC) $(CFLAGS) $< -o $@

$(BINDIR)/server: $(SRCDIR)/server.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: test report clean

test: $(BINDIR)/test

$(BINDIR)/test: $(TSTDIR)/test.c
	$(CC) $(CFLAGS) $< -o $@

report: $(REPORTDIR)/report.tex
	$(TEX) -output-directory $(BINDIR) $^

clean:
	rm -f $(wildcard $(BINDIR)/*)
