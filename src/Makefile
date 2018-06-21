CXXFLAGS=-std=c++11 -I. -Wall -Werror --pedantic-errors -fPIC -O2 \
-Wsign-compare -Wpointer-arith -Wswitch-default -Wswitch-enum -Wshadow
CXX=c++

dsaext-obj := dsaext.o integerparse.o

all: dsaext

dsaext: $(dsaext-obj)

clean:
	rm -f $(dsaext-obj)

distclean: clean

Makefile: ;

.PHONY: all clean distclean dsaext

