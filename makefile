TARGET=CliTest
IDIR=CliMod
CC=gcc
CFLAGS=-I$(IDIR)

SOURCES=main.c CliMod/CliMod.c CliMod/CliCallbacks.c
HEADERS=CliMod.h
#$(info DEPS="$(DEPS)") 

ODIR=obj
LDIR= 		#library directories

LIBS= 		#e.g. math library => -lm 

_DEPS = $(HEADERS)
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = $(patsubst %.c,%.o,$(SOURCES))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

dir_guard=@mkdir -p $(@D)

$(ODIR)/%.o: %.c $(DEPS)
	$(dir_guard)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -fr $(OBJ) *~ core $(INCDIR)/*~ ./$(TARGET)

.PHONY: all

all: CliTest

.PHONY: rebuild

rebuild: clean CliTest
