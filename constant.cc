/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <iostream>
#include <iomanip>
#include <memory>
#include <string>

#include <fcntl.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#define open _open
#define close _close
#define read _read
#define fileno _fileno
#endif

#ifndef _WIN32
#include <sys/mman.h>
#endif // _WIN32

#include "crypto_constant.h"

using namespace std;

int main(int argc, char* const argv[])
{
  if (argc != 3) {
    cerr << "Invalid number of arguments" << endl;
    return -1;
  }
  try {
    if (!crypto::constant::compare(argv[1], argv[2])) {
      cerr << "Strings differ" << endl;
      return 1;
    }
    auto r1 = crypto::constant::Result(argv[1]);
    auto r2 = crypto::constant::Result(argv[2]);
    if (r1 != r2) {
      cerr << "Results differ" << endl;
      return 1;
    }
    return 0;
  }
  catch (std::exception& ex) {
    cerr << "Error: " << ex.what() << endl;
    return 2;
  }
}
