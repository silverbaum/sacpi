#make sacpi

###
PREFIX=/usr/local
###
CFLAGS= -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -pedantic
CC=cc

sacpi : sacpi.c
	$(CC) $(CFLAGS) -o sacpi sacpi.c
#gnu extension for case-insensitive strcasestr
gnu:
	$(CC) $(CFLAGS) -D_GNU_SOURCE -o sacpi sacpi.c
install: 
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f sacpi $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/sacpi
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/sacpi
clean:
	rm sacpi
