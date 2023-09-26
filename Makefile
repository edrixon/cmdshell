CC=gcc
CFLAGS=-Wall
LIB=-lreadline
OBJS=main.o
INC=config.h cli.h commands.h types.h
EXEC=cmdshell
BINDIR=/opt/radio/sbin

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXEC) $(OBJS) $(LIB)

%.o: %.c $(INC)
	$(CC) -c -o $@ $< $(CFLAGS)

install: $(EXEC)
	cp $(EXEC) $(BINDIR)
	chmod 755 $(BINDIR)/$(EXEC)

clean:
	rm -f $(OBJS)
	rm -f $(EXEC)
