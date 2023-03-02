src=$(wildcard src/*.c)
obj=$(patsubst %.c,%.o,$(src))
.PHONY: all clean install
all: testtp
testtp: $(obj)
	gcc -o $@ $^ -pthread
%.o: %.c
	gcc -c -g -Iinclude -o $@ $<
install:
	mv testtp target
clean:
	find src ! -name src ! -name *.c -exec rm -rf {} \;
	rm -rf target/*