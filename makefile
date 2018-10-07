CC=g++
OUT=-o
BIN=rsyncez

SOURCE=rsyncez.cpp 
HEADER=json.hpp

all: $(SOURCE) $(HEADER)
	$(CC) $(OUT) $(BIN) $(SOURCE)