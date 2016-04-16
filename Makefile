.PHONY: all

all: linwrapper.so

linwrapper.so: linwrapper.c
	$(CC) -shared -Wall -pedantic -O3 -fPIC $^ -ldl -o $@

clean:
	rm -f linwrapper.so
