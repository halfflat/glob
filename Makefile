.PHONY: clean all
.SECONDARY:

top:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))

all:: unit

test-src:=unit.cc
test-obj:=$(patsubst %.cc, %.o, $(test-src))

depends:=$(patsubst %.cc, %.d, $(test-src))

gtest-top:=$(top)test/googletest/googletest
gtest-inc:=$(gtest-top)/include
gtest-src:=$(gtest-top)/src/gtest-all.cc

vpath %.cc $(top)test

OPTFLAGS?=-O3 -march=native
CXXFLAGS+=$(OPTFLAGS) -MMD -MP -std=c++14 -g -pthread
CPPFLAGS+=-isystem $(gtest-inc) -I $(top)include

-include $(depends)

gtest.o: CPPFLAGS+=-I $(gtest-top)
gtest.o: ${gtest-src}
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

unit: $(test-obj) gtest.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(test-obj)

realclean: clean
	rm -f unit gtest.o $(depends)
