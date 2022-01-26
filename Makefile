PWD = $(shell pwd)
BIN_DIR = $(PWD)/bin
TARGET = exec
DESTINATION := $(BIN_DIR)/$(TARGET)

RM := rm
SRC_SUFFIX = cc
CXX := clang++
CPPFLAGS = -std=c++11 -Wall -g

SRC := $(wildcard *.$(SRC_SUFFIX))
OBJS := $(patsubst %.$(SRC_SUFFIX), %.o, $(SRC))

.PHONY: build clean

build: $(TARGET)

make_dir:
	mkdir -p $(BIN_DIR)

$(TARGET): make_dir $(OBJS)
	$(CXX) -o $(DESTINATION) $(OBJS) $(addprefix -l, $(LIBS))

clean:
	$(RM) *.o
	$(RM) $(DESTINATION)