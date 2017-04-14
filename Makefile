CC=clang
CFLAGS=-Wall -Werror -Wno-unused-const-variable -pedantic -Wno-gnu $(shell sdl2-config --cflags)
LFLAGS=$(shell sdl2-config --libs)

SRCDIR=src
BINDIR=bin
OBJDIR=$(BINDIR)/obj

TARGET=$(BINDIR)/clife
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJFILES=$(patsubst %.c,%.o,$(subst $(SRCDIR),$(OBJDIR),$(SOURCES)))
OBJECTS=$(patsubst %$(SRCDIR)/,$(OBJDIR)/%,$(OBJFILES))

all: $(TARGET)

$(TARGET): $(OBJECTS) tags
	$(CC) $(LFLAGS) -o $(TARGET) $(OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BINDIR):
	mkdir -p $@

$(OBJDIR):
	mkdir -p $@

clean:
	-rm -f $(OBJECTS)
	-rm -f tags cscope.files cscope.out

distclean:
	-rm -f $(TARGET)

tags:
	cd $(SRCDIR)
	ctags -eR --exclude=Makefile --exclude=bin/*
	find . \( -name "*.c" -o -name "*.h" \) > cscope.files; cscope -b
	cd ..

.PHONY: all clean distclean tags
