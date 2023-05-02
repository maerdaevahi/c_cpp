SRC_DIR := src
TARGET_DIR := target
INCLUDE := include
CC := g++

srcs := $(wildcard $(SRC_DIR)/*.cc)
objs := $(patsubst $(SRC_DIR)/%.cc,$(TARGET_DIR)/%.o,$(srcs))
apps := $(patsubst $(SRC_DIR)/%.cc,$(TARGET_DIR)/%,$(srcs))

.PHONY: all clean
all: $(apps)

%: %.o
	$(CC) -o $@ $<
$(objs): $(TARGET_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CC)	-c -g -o $@ $<
clean:
	rm -rf $(objs) $(apps)

