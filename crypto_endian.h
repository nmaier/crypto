/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#ifndef CRYPTO_ENDIAN_H
#define CRYPTO_ENDIAN_H

#include <cstdint>

namespace crypto {

#if defined(__GNUG__)
#define forceinline __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#define forceinline __forceinline
#else // ! _MSC_VER
#define forceinline inline
#endif // ! _MSC_VER

/* In order for this implementation to work your system (or you yourself) must
 * define after including <sys/param.h>
 *   - LITTLE_ENDIAN
 *   - BIG_ENDIAN
 *   - BYTE_ORDER
 *   - where BYTE_ORDER == LITTLE_ENDIAN or BYTE_ORDER == BIG_ENDIAN
 * Failing to conform will render this implementation utterly incorrect.
 */
#if defined(_WIN32) || defined(__INTEL_COMPILER) || defined (_MSC_VER)
// Itanium is dead!
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#define BYTE_ORDER LITTLE_ENDIAN
#else // !  defined(_WIN32) || defined(__INTEL_COMPILER) || defined (_MSC_VER)
#include <sys/param.h>
#endif // !  defined(_WIN32) || defined(__INTEL_COMPILER) || defined (_MSC_VER)

#if !defined(LITTLE_ENDIAN) || !defined(BIG_ENDIAN) || !defined(BYTE_ORDER) || (LITTLE_ENDIAN != BYTE_ORDER && BIG_ENDIAN != BYTE_ORDER)
#error Unsupported byte order/endianess
#endif

// Lets spend some quality time mucking around with byte swap and endian-ness.
// First bswap32:
#if (defined(__i386__) || defined(__x86_64__)) && defined(__GNUG__)
forceinline uint32_t __crypto_bswap32(uint32_t p)
{
    __asm__ __volatile__("bswap %0" : "=r"(p) : "0"(p));
    return p;
}
#elif defined(__GNUG__)
forceinline uint32_t __crypto_bswap32(uint32_t p) {
  return __builtin_bswap32(p);
}
#else // defined(__GNUG__)
forceinline uint32_t __crypto_bswap32(uint32_t n)
{
  n = ((n << 8) & 0xff00ff00) | ((n >> 8) & 0xff00ff);
  return (n << 16) | (n >> 16);
}
#endif // defined(__GNUG__)

// Next up: bswap64
#if defined(__x86_64__) && defined(__GNUG__)
forceinline uint64_t __crypto_bswap64(uint64_t p)
{
  __asm__ __volatile__("bswapq %q0" : "=r"(p) : "0"(p));
  return p;
}
#elif defined(__GNUG__)
forceinline uint64_t __crypto_bswap64(uint64_t p) {
  return __builtin_bswap64(p);
}
#else // defined(__GNUG__)
forceinline uint64_t __crypto_bswap64(uint64_t n)
{
  n = ((n << 8) & 0xff00ff00ff00ff00) | ((n >> 8) & 0x00ff00ff00ff00ff);
  n = ((n << 16) & 0xffff0000ffff0000) | ((n >> 16) & 0x0000ffff0000ffff);
  return (n << 32) | (n >> 32);
}
#endif // defined(__GNUG__)

// Time for an implementation that makes reuse easier.
namespace {
template<typename T>
inline T __crypto_bswap(T n)
{
  static_assert(sizeof(T) != sizeof(T), "Not implemented");
}

template<>
inline uint32_t __crypto_bswap(uint32_t n)
{
  return __crypto_bswap32(n);
}

template<>
inline uint64_t __crypto_bswap(uint64_t n)
{
  return __crypto_bswap64(n);
}
} // namespace

// __crypto_le and __crypto_be depending on byte order
#if LITTLE_ENDIAN == BYTE_ORDER
template<typename T>
forceinline T __crypto_be(const T n) {
  return __crypto_bswap(n);
}
template<typename T>
forceinline T __crypto_le(const T n) {
  return n;
}
#else // LITTLE_ENDIAN != WORD_ORDER
template<typename T>
forceinline T __crypto_be(const T n) {
  return n;
}
template<typename T>
forceinline T __crypto_le(const T n) {
  return __crypto_bswap(n);
}
#endif // LITTLE_ENDIAN != WORD_ORDER

} // namespace crypto

#endif // CRYPTO_ENDIAN_H
