CPP=g++
CPPFLAGS=-Wall -Wextra -pedantic -std=c++11 -Itclap-1.2.1/include

SOURCES=text2bin.cpp bin2text.cpp ext_sort.cpp

BIN_SORT=ext_sort
BIN_BIN=bin2text
BIN_TEXT=text2bin

all: $(SOURCES) $(BIN_SORT) $(BIN_TEXT) $(BIN_BIN)

$(BIN_SORT): ext_sort.cpp
	$(CPP) $(CPPFLAGS) ext_sort.cpp -o $@

$(BIN_TEXT): text2bin.cpp
	$(CPP) $(CPPFLAGS) text2bin.cpp -o $@

$(BIN_BIN): bin2text.cpp
	$(CPP) $(CPPFLAGS) bin2text.cpp -o $@

clean:
	rm $(BIN_SORT) $(BIN_TEXT) $(BIN_BIN)

