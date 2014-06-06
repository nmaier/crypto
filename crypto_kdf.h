/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#ifndef CRYPTO_KDF_H
#define CRYPTO_KDF_H

#include <cstdint>
#include <string>
#include <stdexcept>

#include "crypto_constant.h"
#include "crypto_hmac.h"

namespace crypto {
namespace kdf {

/**
 * Salted HKDF, as per RFC 5869.
 * This is a combination of the Extract and Expand steps.
 */
constant::Result HKDF(hash::Algorithms algorithm,
                      const char* salt,
                      size_t salt_length,
                      const char* keymaterial,
                      size_t km_length,
                      const char* info,
                      size_t info_length,
                      uint32_t desired_length);

inline constant::Result HKDF(hash::Algorithms algorithm,
                             const std::string& salt,
                             const std::string& keymaterial,
                             const std::string& info,
                             uint32_t desired_length)
{
  return HKDF(algorithm,
              salt.data(),
              salt.length(),
              keymaterial.data(),
              keymaterial.length(),
              info.data(),
              info.length(),
              desired_length);
}

/**
 * Non-Salted HKDF, as per RFC 5869.
 * This is just the Expand step.
 */
constant::Result HKDF(hash::Algorithms algorithm,
                      const char* key,
                      const size_t key_length,
                      const char* info,
                      size_t info_length,
                      uint32_t desired_length);

inline constant::Result HKDF(hash::Algorithms algorithm,
                             const std::string& key,
                             const std::string& info,
                             uint32_t desired_length)
{
  return HKDF(algorithm,
              key.data(),
              key.length(),
              info.data(),
              info.length(),
              desired_length);
}

/**
 * Create A PKBDF2-HMAC. See RFC 2898.
 */
constant::Result PBKDF2(hash::Algorithms algorithm,
                        const char* password,
                        size_t password_length,
                        const char* salt,
                        size_t salt_length,
                        size_t iterations,
                        uint32_t desired_length);

/**
 * Create A PKBDF2-HMAC. See RFC 2898.
 */
inline constant::Result PBKDF2(hash::Algorithms algorithm,
                               const std::string& password,
                               const std::string& salt,
                               size_t iterations,
                               uint32_t desired_length)
{
  return PBKDF2(algorithm,
                password.data(),
                password.length(),
                salt.data(),
                salt.length(),
                iterations,
                desired_length);
}


} // namespace constant
} // namespace crypto

#endif // CRYPTO_HMAC_H
