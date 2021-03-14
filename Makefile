DESTINATION := ./bin/exec

RM := rm
SRC_SUFFIX = cc
CC = g++
CPPFLAGS = -std=c++11 -Wall -g

SRC := $(wildcard *.$(SRC_SUFFIX))
OBJS := $(patsubst %.$(SRC_SUFFIX), %.o, $(SRC))

.PHONY: all clean rebuild

rebuild: clean all

all: $(DESTINATION)

$(DESTINATION): $(OBJS)
	$(CC) -o $(DESTINATION) $(OBJS) $(addprefix -l,$(LIBS))

clean:
	$(RM) *.o
	$(RM) $(DESTINATION)