CC = gcc
CCFLAGS = -O3 -Wall -Wextra -Wpedantic
LDFLAGS = -flto
LD = gcc

APPNAME = hxc.exe

all: $(APPNAME)

$(APPNAME): hxc.o buffer.o
	$(LD) $(LDFLAGS) -o $@ $^

hxc.o: src/hxc.c src/hxc.h
	$(CC) $(CCFLAGS) -o $@ -c $<

buffer.o: src/buffer.c src/buffer.h
	$(CC) $(CCFLAGS) -o $@ -c $<