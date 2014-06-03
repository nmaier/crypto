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

hash_OBJS = hash.o

hmac_OBJS = hmac.o

constant_OBJS = constant.o

OBJS= \
			crypto_hash.o\
			crypto_hmac.o\


HASH_CHECKS = \
							.md5.hashcheck\
							.sha1.hashcheck\
							.sha224.hashcheck\
							.sha256.hashcheck\
							.sha384.hashcheck\
							.sha512.hashcheck\

HMAC_CHECKS = \
							.md5.hmaccheck\
							.sha1.hmaccheck\
							.sha224.hmaccheck\
							.sha256.hmaccheck\
							.sha384.hmaccheck\
							.sha512.hmaccheck\


all:: check

.cc.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

libnmcrypto.a: $(OBJS)
	$(AR) cr $@ $^

constant: $(constant_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

check:: constant
	./constant "" ""
	./constant abc abc
	./constant abc cde; test "$$?" -eq 1
	./constant abcd abc; test "$$?" -eq 2
	@echo '------------------'
	@echo 'SUCCESS (constant)'
	@echo '------------------'

hash: $(hash_OBJS) libnmcrypto.a
	$(CXX) $(CXXFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

.%.hashp1: hash
	@echo -n $@
	@$(TIMECMD) ./hash -a $(subst .,,$(basename $@)) $F > $@

.%.hashp2:
	@echo -n $@
	@$(TIMECMD) python check_hash.py -a $(subst .,,$(basename $@)) $F > $@

.%.hashcheck: .%.hashp1 .%.hashp2
	diff -U8 $^ > $@
	@rm -rf $^
	@echo

check:: hash $(HASH_CHECKS)
	@rm -rf $(HASH_CHECKS)
	@echo '--------------'
	@echo 'SUCCESS (hash)'
	@echo '--------------'

hmac: $(hmac_OBJS) libnmcrypto.a
	$(CXX) $(CXXFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

.%.hmacp1: hash
	@./hmac -a $(subst .,,$(basename $@)) $F > $@
	@./hmac -s abc -a $(subst .,,$(basename $@)) $F >> $@
	@./hmac -s password12345 -a $(subst .,,$(basename $@)) $F >> $@
	@./hmac -s 'pass\0word' -a $(subst .,,$(basename $@)) $F >> $@
	@./hmac -s aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa  -a $(subst .,,$(basename $@)) $F >> $@
	@echo Made $@

.%V.hashp2:
	$(TIMECMD) python check_hash.py -a $(subst .,,$(basename $@)) $F > $@

.%.hmacp2: hmac
	@python check_hmac.py -a $(subst .,,$(basename $@)) $F >> $@
	@python check_hmac.py -s abc -a $(subst .,,$(basename $@)) $F >> $@
	@python check_hmac.py -s password12345 -a $(subst .,,$(basename $@)) $F >> $@
	@python check_hmac.py -s 'pass\0word' -a $(subst .,,$(basename $@)) $F >> $@
	@python check_hmac.py -s aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa  -a $(subst .,,$(basename $@)) $F >> $@
	@echo Made $@


.%.hmaccheck: .%.hmacp1 .%.hmacp2
	diff -U8 $^ > $@
	@rm -rf $^
	@echo

check:: hmac $(HMAC_CHECKS)
	@rm -rf $(HMAC_CHECKS)
	@echo '--------------'
	@echo 'SUCCESS (hmac)'
	@echo '--------------'

clean:
	@rm -f $(constant_OBJS)
	@rm -f $(hash_OBJS)
	@rm -f $(hmac_OBJS)
	@rm -f $(OBJS)
	@rm -f $(HASH_CHECKS)
	@rm -f $(HMAC_CHECKS)
	@rm -f .*.*p1
	@rm -f .*.*p2
	@rm -f libnmcrypto.a
	@rm -f constant hash hmac

.PHONY: all clean check .%.hashcheck .%.hashp1 .%.hashp2
