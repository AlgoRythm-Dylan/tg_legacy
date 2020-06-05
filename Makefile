CC=gcc
CFLAGS=-D_GNU_SOURCE
ODIR=obj
LIBS=-lncursesw -ltinfo
DEPS=tg.h tgsys.h tgdraw.h
INC=-I/usr/include/ncursesw

all: tgtest clean

$(shell mkdir -p $(ODIR))

_OBJ=tg.o tgsys.o tgdraw.o tgtest.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(INC)

tgtest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INC) $(LIBS)

clean:
	$(shell rm -rf $(ODIR))