.PHONY: clean all
.SECONDARY:

top:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))

all:: unit libglob.a

glob-src:=match.cc runglob.cc
glob-obj:=$(patsubst %.cc, %.o, $(glob-src))

test-src:=unit.cc test_match_backtrack.cc test_match_nfa.cc
test-obj:=$(patsubst %.cc, %.o, $(test-src))

gtest-top:=$(top)test/googletest/googletest
gtest-inc:=$(gtest-top)/include
gtest-src:=$(gtest-top)/src/gtest-all.cc

depends:=gtest.d $(patsubst %.cc, %.d, $(glob-src) $(test-src))

vpath %.cc $(top)glob
vpath %.cc $(top)test

OPTFLAGS?=-O3 -march=native
CXXFLAGS+=$(OPTFLAGS) -MMD -MP -std=c++17 -g -pthread
CPPFLAGS+=-isystem $(gtest-inc) -I $(top)
LDLIBS+=-lstdc++fs
ARFLAGS+=-U

-include $(depends)

gtest.o: CPPFLAGS+=-I $(gtest-top)
gtest.o: ${gtest-src}
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

libglob.a: libglob.a($(glob-obj))

unit: $(test-obj) gtest.o libglob.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f unit libglob.a $(glob-obj) $(test-obj)

realclean: clean
	rm -f gtest.o $(depends)
