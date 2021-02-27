
build: src/raspfan.c
	mkdir -p bin
	gcc -o bin/raspfan src/raspfan.c -lwiringPi

FORCE:
clean:
	rm bin/raspfan

install:
	cp raspfan.service /etc/systemd/system/
	cp bin/raspfan /usr/local/bin/
