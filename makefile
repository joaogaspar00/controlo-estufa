# Variables and directories
CC= gcc
CFLAGS= -Wall -g -pthread
LDFLAGS= -lpthread -lrt
COMUNICATIONS_FILES = ./src/procCom/*.c

# Program compilation
controlo-estufa: sismon intuti reghist
sismon:
	$(CC) $(CFLAGS) ./src/sismon/*.c $(COMUNICATIONS_FILES) -o bin/sismon $(LDFLAGS)
intuti:
	$(CC) $(CFLAGS) ./src/intuti/*.c $(COMUNICATIONS_FILES) -o bin/intuti $(LDFLAGS)
reghist:
	$(CC) $(CFLAGS) ./src/reghist/*.c $(COMUNICATIONS_FILES) -o bin/reghist $(LDFLAGS)

# Files cleaning
rmexe:
	rm -f bin/*
rmsockets:
	rm /tmp/INTUTI
	rm /tmp/SISMON
	rm /tmp/REGS

