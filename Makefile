#make sacpi

###
PREFIX=/usr/local
###
CFLAGS= -Wall -pedantic
CC=cc

sacpi : sacpi.c
	$(CC) $(CFLAGS) -o sacpi sacpi.c
install: 
	mkdir -p $(PREFIX)/bin
	cp -f sacpi $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/sacpi
uninstall:
	rm $(PREFIX)/bin/sacpi
clean:
	rm sacpi
