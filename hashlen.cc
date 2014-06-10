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

#include "crypto_hash.h"

using namespace std;

int main(int argc, char* const argv[])
{
  cout << argv[1] << " " << crypto::hash::create(argv[1])->length() << endl;
  return 0;
}
