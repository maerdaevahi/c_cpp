src=$(wildcard src/*.c)
obj=$(patsubst %.c,%.o,$(src))
all: testmmap
testmmap: $(obj)
	gcc -o $@ $^
%.o: %.c
	gcc -c -g -Iinclude -o $@ $<
install:
	mv testmmap target
clean:
	find src ! -name src ! -name *.c -exec rm -rf {} \;
	rm -rf target/*
.PHONY: all clean install