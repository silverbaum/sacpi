###
PREFIX=/usr/local
###
CFLAGS= -Wall -O2 -D_POSIX_C_SOURCE_200809L -std=gnu99
CC=cc

sacpi : sacpi.c
	$(CC) $(CFLAGS) -o sacpi sacpi.c
install: sacpi
	mkdir -p $(PREFIX)/bin
	cp -f sacpi $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/sacpi
uninstall:
	rm $(PREFIX)/bin/sacpi
clean:
	rm sacpi
