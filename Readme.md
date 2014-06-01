Various hash functions
===

The implemenation is supposed to be reasonably portable, incl. little endian
and big endian systems, and reasonably fast.

Hashes
---
* `MD5`
* `SHA1`
* `SHA224`
* `SHA256`

Compatiblity
---
The code is `C++11` code without any external dependencies.

Test Matrix
---

All algorithms will be tested by issueing `make check`, which happens to be the
default target.
The test will by default hash all `hash*` files (aka. the source and binaries)
plus somme vectors in `test-vectors`and compare the output against what the
Python `hashlib` produces.

Passing tests:

* `Debian lenny x86_64 le, gcc-4.7`
* `Debian wheezy mips/manta be, gcc-4.7
* `OSX 10.9/Darwin x86_64 le, clang-apple-5.1`
* `OSX 10.9/Darwin i686 le, clang-apple-5.1`
* `OSX 10.9/Darwin x86_64 le, gcc-4.8`
* `OSX 10.9/Darwin i686 le, gcc-4.8`
* `Windows x86_64-w64-mingw32 gcc-4.8.3`
* `Windows WIN32 MSVC12u2` (create your own `.sln`)
* `Windows WIN64 MSVC12u2` (create your own `.sln`)

License
---
Well...
```
Any copyright is dedicated to the Public Domain.
http://creativecommons.org/publicdomain/zero/1.0/
Written in 2014 by Nils Maier
```
