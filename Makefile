
all:
	mkdir -p bin
	gcc -o bin/fan src/fan.c -lwiringPi

FORCE:
clean:
	rm bin/fan
