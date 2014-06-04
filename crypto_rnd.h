/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#ifndef CRYPTO_RND_H
#define CRYPTO_RND_H

#include <cstdint>
#include <string>
#include <stdexcept>

namespace crypto {
namespace rnd {

uint8_t* randomBytes(uint8_t* bytes, size_t len);

template<typename T>
inline T random()
{
  T rv;
  uint8_t* bytes = reinterpret_cast<uint8_t>(&rv);
  randomBytes(bytes, sizeof(rv));
  return rv;
}

} // rnd
} // crypto

#endif // CRYPTO_RND_H
