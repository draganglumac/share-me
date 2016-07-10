all: clean share

clean:
	-rm share
	-rm share.dSYM

share:
	cc -pthread -g -o share main.c

