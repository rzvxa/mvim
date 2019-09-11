mvim: src/mvim.c
	clang -o bin/mvim src/mvim.c -Isrc -lpcre

PREFIX ?= /usr

all:
	$(PREFIX)

install: bin/mvim
	install -m 755 bin/mvim $(PREFIX)/bin

uninstall: bin/mvim
	rm -rf $(PREFIX)/bin/mvim
