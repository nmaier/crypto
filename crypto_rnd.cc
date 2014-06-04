/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#include "crypto_rnd.h"

#include <limits>
#include <algorithm>

#if defined(_WIN32) || defined(_MSC_VER)
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#else // ! defined(_WIN32) || defined(_MSC_VER)
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#endif // ! defined(_WIN32) || defined(_MSC_VER)

// Implement our "generator" only with proven OS-level stuff.
uint8_t* crypto::rnd::randomBytes(uint8_t* bytes, size_t len)
{
  if (!len) {
    return bytes;
  }
#if defined(_WIN32) || defined(_MSC_VER)
  static HCRYPTPROV prov = 0;
  if (!prov &&
      !CryptAcquireContext(
           &prov, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
    throw std::runtime_error("Cannot aquire random provider");
  }
  auto p = bytes;
  while (len) {
    const DWORD turn = std::min(len, (size_t)std::numeric_limits<DWORD>::max());
    if (!CryptGenRandom(prov, turn, p)) {
      throw std::runtime_error("Failed to get required number of random bytes");
    }
    p += turn;
    len -= turn;
  }
#else // ! defined(_WIN32) || defined(_MSC_VER)
  static int fd = -1;
  auto p = bytes;
  if (fd < 0) {
#ifdef __linux__
    // On linux we want to make sure the random pool is seeded first.
    // Thereofre we make a single read from /dev/random.
    if ((fd = open("/dev/random", O_RDONLY)) < 0) {
      throw std::runtime_error("Failed to open /dev/random");
    }
    int initial;
    while ((initial = read(fd, p, 1)) <= 0 && errno == EINTR) {}
    close(fd);
    fd = -1;

    if (initial <= 0) {
      throw std::runtime_error("Failed to read from /dev/random");
    }
    p += 1;
    len -= 1;
#endif // __linux__
    if ((fd = open("/dev/urandom", O_RDONLY)) < 0) {
      throw std::runtime_error("Failed to open /dev/urandom");
    }
  }
  while (len) {
    const int turn = std::min(len, (size_t)std::numeric_limits<int>::max());
    int r;
    while ((r = read(fd, p, turn)) <= 0 && errno == EINTR) {}
    if (r <= 0) {
      throw std::runtime_error("Failed to get required number of random bytes");
    }
    p += r;
    len -= r;
  }
#endif // ! defined(_WIN32) || defined(_MSC_VER)
  return bytes;
}
