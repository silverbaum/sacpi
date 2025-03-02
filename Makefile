#sacpi

CFLAGS= -std=c99 -Wall -pedantic 
CC=cc

sacpi : 
	$(CC) $(CFLAGS) -o sacpi sacpi.c
install: sacpi
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f sacpi $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/sacpi
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/sacpi
