#sacpi

CFLAGS= -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -pedantic #-D_GNU_SOURCE
CC=cc

sacpi : 
	$(CC) $(CFLAGS) -o sacpi sacpi.c
install: sacpi
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f sacpi $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/sacpi
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/sacpi
