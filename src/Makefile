CC=g++
CCFLAGS=-Wall -Werror -lpthread -g
INCLUDE=../include
OUTPUT=../bin

OBJECT_SRC= \
	extractor.cc \
	timeline.cc
OBJECTS=$(wildcard $(OUTPUT)/*.o)

SRC= \
	main.cc
EXEC= \
	$(OUTPUT)/run

.PHONY: compile $(OBJECT_SRC)
compile: $(OBJECT_SRC)
	$(CC) $(CCFLAGS) -o $(EXEC) -I$(INCLUDE) $(OBJECTS) $(SRC)
$(OBJECT_SRC):
	$(CC) $(CCFLAGS) -c -o $(OUTPUT)/$(basename $@).o -I$(INCLUDE) $@
