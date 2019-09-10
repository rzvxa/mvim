mvim: src/mvim.c
	gcc -o bin/mvim src/mvim.c

PREFIX ?= /usr

all:
	$(PREFIX)

install: bin/mvim
	install -m 755 bin/mvim $(PREFIX)/bin

uninstall: bin/mvim
	rm -rf $(PREFIX)/bin/mvim
