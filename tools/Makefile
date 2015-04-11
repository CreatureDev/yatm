.c.o:
	$(CC) $(CLFG) -c $^
CC=gcc
CFLG=-Wall -ggdb
LFLG=
OBJ=
PRG=objtocmb

all:$(PRG)

objtocmb: objtocmb.o objutil.o objvertlist.o
	$(CC) $(CFLG) $(LFLG) -o $@ $^


clean:
	/bin/rm -f $(PRG) *.o a.out
