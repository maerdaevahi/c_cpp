src=$(wildcard src/*.c)
obj=$(patsubst %.c,%.o,$(src))
all: testpthread
testpthread: $(obj)
	gcc -o $@ $^ -pthread
%.o: %.c
	gcc -c -g -Iinclude -o $@ $<
install:
	mv testpthread target
clean:
	find src ! -name src ! -name *.c -exec rm -rf {} \;
	rm -rf target/*
.PHONY: all clean install