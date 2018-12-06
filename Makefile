.PHONY: clean all
.SECONDARY:

top:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))

all:: unit glob libglob.a

glob-src:=match.cc glob.cc

ifdef with-std-filesystem
CXXFLAGS+=-std=c++17
CPPFLAGS+=-DHF_GLOB_USE_STD_FILESYSTEM=1
glob-src+=glob_std_fs_provider.cc
else
CXXFLAGS+=-std=c++14
glob-src+=glob_posix_fs_provider.cc
endif

glob-obj:=$(patsubst %.cc, %.o, $(glob-src))

test-src:=unit.cc test_match_backtrack.cc test_match_nfa.cc test_glob.cc
test-obj:=$(patsubst %.cc, %.o, $(test-src))

glob-cli-src:=glob-cli.cc
glob-cli-obj:=$(patsubst %.cc, %.o, $(glob-cli-src))

gtest-top:=$(top)test/googletest/googletest
gtest-inc:=$(gtest-top)/include
gtest-src:=$(gtest-top)/src/gtest-all.cc

depends:=gtest.d $(patsubst %.cc, %.d, $(glob-src) $(test-src))

vpath %.cc $(top)glob
vpath %.cc $(top)src
vpath %.cc $(top)match
vpath %.cc $(top)test

OPTFLAGS?=-O3 -march=native
CXXFLAGS+=$(OPTFLAGS) -MMD -MP -g -pthread
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

glob: $(glob-cli-obj) libglob.a
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f unit libglob.a $(glob-obj) $(test-obj) $(glob-cli-obj)

realclean: clean
	rm -f gtest.o $(depends)
