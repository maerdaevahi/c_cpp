src=$(wildcard src/*.c)
obj=$(patsubst %.c,%.o,$(src))
all: testsockettcp
testsockettcp: $(obj)
	gcc -o $@ $^
%.o: %.c
	gcc -c -g -Iinclude -o $@ $<
install:
	mv testsockettcp target
clean:
	find src ! -name src ! -name *.c -exec rm -rf {} \;
	rm -rf target/*
.PHONY: all clean install