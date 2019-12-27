CC=gcc
CFLAGS=-I.
ODIR=obj
LIBS=-lncursesw
DEPS=tg.h

_OBJ=tg.o tgtest.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tgtest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

all: tgtest clean

clean:
	rm $(ODIR)/*.o