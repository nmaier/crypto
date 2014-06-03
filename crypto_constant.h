/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#ifndef CRYPTO_CONSTANT_H
#define CRYPTO_CONSTANT_H

#include <cstdint>
#include <string>
#include <stdexcept>

namespace crypto {
namespace constant {

/**
 * Compare two bytes in constant time.
 *
 * @param a First byte.
 * @param b Second byte.
 * @return True, if both match, false otherwise.
 */
inline bool compare(const uint8_t a, const uint8_t b)
{
  uint8_t rv = ~(a ^ b);
  rv &= rv >> 4;
  rv &= rv >> 2;
  rv &= rv >> 1;
  return !!rv;
}

/**
 * Compare two byte arrays in constant time. The arrays must have the same
 * length!
 *
 * @param a First byte array.
 * @param b Second byte array.
 * @return True, if both match, false otherwise.
 */
inline bool compare(const uint8_t* a, const uint8_t* b, size_t length)
{
  unsigned char rv = 0;
  for (size_t i = 0; i < length; ++i) {
    rv |= a[i] ^ b[i];
  }
  return compare(rv, 0);
}

inline bool compare(const char* a, const char* b, size_t length)
{
  return compare(reinterpret_cast<const uint8_t*>(a),
                 reinterpret_cast<const uint8_t*>(b),
                 length * sizeof(char));
}

inline bool compare(const std::string& a, const std::string& b)
{
  union {
    std::string::size_type length;
    uint8_t b[sizeof(std::string::size_type) / sizeof(uint8_t)];
  } la = { a.length() }, lb = { b.length() };
  if (!compare(la.b, lb.b, sizeof(la))) {
    throw std::domain_error("crypto::constant: length mismatch");
  }

  return compare(reinterpret_cast<const uint8_t*>(a.data()),
                 reinterpret_cast<const uint8_t*>(b.data()),
                 la.length * sizeof(char));
}

/**
 * Result wrapper. While it is still possible to get the raw result bytes,
 * when using this wrapper it is ensured that constant-time comparison is used.
 * Also, this wrapper makes it an error to compare results of a different
 * length, helping to prevent logic errors either during development, or
 * triggering in the wild. Therefore |.bytes()| use should be avoided.
 */
class Result
{
public:
  Result(const std::string& result) : result_(result)
  {}

  Result(const char* result, size_t length)
    : result_(result, length)
  {}

  Result(const Result& other) = default;

#ifndef _MSC_VER
  Result(Result&& other) = default;
#endif // _MSC_VER

  Result& operator=(const Result& other) = default;

#ifndef _MSC_VER
  Result& operator=(Result&& other) = default;
#endif // _MSC_VER

  bool operator==(const Result& other) const
  {
    return crypto::constant::compare(result_, other.result_);
  }

  bool operator!=(const Result& other) const
  {
    return !(*this == other);
  }

  size_t length() const
  {
    return result_.length();
  }

  const std::string& bytes() const
  {
    return result_;
  }

private:
  std::string result_;
};

} // namespace constant
} // namespace crypto

#endif // CRYPTO_HMAC_H
