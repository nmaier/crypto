UNAME := $(shell uname)
CXX ?= c++
ifdef _DEBUG
CXXFLAGS ?= -std=c++11 -O0 -g -march=native -Werror $(ARCH)
else
CXXFLAGS ?= -std=c++11 -Os -march=native -Werror $(ARCH)
endif
COMPILER := $(shell $(CXX) -v 2>&1)
F = *.cc *.h *.o *.a hash* test-vectors/*

ifneq (,$(findstring clang,$(COMPILER)))
CXXFLAGS += -stdlib=libc++
endif

ifneq ($(UNAME),Linux)
TIMECMD ?= time
else
TIMECMD ?= time -v
endif

hash_OBJS = main.o

HASH_OBJS = \
						crypto_hash.o\


HASH_CHECKS = \
							.md5.check\
							.sha1.check\
							.sha224.check\
							.sha256.check\
							.sha384.check\
							.sha512.check\


all:: check

.cc.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

libnmcryptohash.a: $(HASH_OBJS)
	$(AR) cr $@ $^

hash: $(hash_OBJS) libnmcryptohash.a
	$(CXX) $(CXXFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

.%.p1: hash
	$(TIMECMD) ./hash -a $(subst .,,$(basename $@)) $F > $@

.%.p2:
	$(TIMECMD) python check.py $(subst .,,$(basename $@)) $F > $@

.%.check: .%.p1 .%.p2
	diff -U8 $^ > $@
	@rm -rf $^

check:: hash $(HASH_CHECKS)
	@rm -rf $(HASH_CHECKS)
	@echo -------
	@echo SUCCESS
	@echo -------

clean:
	@rm -f $(hash_OBJS)
	@rm -f $(HASH_OBJS)
	@rm -f $(HASH_CHECKS)
	@rm -f libnmcryptohash.a
	@rm -f hash

.PHONY: all clean check .%.check .%.p1 .%.p2
