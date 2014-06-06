/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#include "crypto_kdf.h"
#include "crypto_endian.h"

#include <algorithm>

using namespace crypto;

constant::Result crypto::kdf::HKDF(hash::Algorithms algorithm,
                                   const char* salt,
                                   size_t salt_length,
                                   const char* keymaterial,
                                   size_t km_length,
                                   const char* info,
                                   size_t info_length,
                                   uint32_t desired_length)
{
  // Step 1 - Extract
  auto h = hmac::create(algorithm, salt, salt_length);
  auto prk = h->finalize(keymaterial, km_length).bytes();

  // Step 2 - Expand
  return HKDF(
      algorithm, prk.data(), prk.length(), info, info_length, desired_length);
}

constant::Result crypto::kdf::HKDF(hash::Algorithms algorithm,
                                   const char* key,
                                   const size_t key_length,
                                   const char* info,
                                   size_t info_length,
                                   uint32_t desired_length)
{
  // Step 2 - Expand
  auto h = hmac::create(algorithm, key, key_length);
  const auto hl = h->length();
  if (desired_length > hl * 255) {
    throw std::domain_error("desired_length exceeds 255 times hash length");
  }
  std::string rv;
  std::string last; // Initially an empty string
  uint8_t ch = 1;
  for (uint32_t i = 0; i < desired_length; i += hl, ++ch) {
    h->update(last);
    h->update(info, info_length);
    h->update((char*)&ch, 1);
    rv.append(last = h->finalize().bytes());
  }
  rv.resize(desired_length);
  return constant::Result(rv);
}

constant::Result crypto::kdf::PBKDF2(hash::Algorithms algorithm,
                                           const char* password,
                                           size_t password_length,
                                           const char* salt,
                                           size_t salt_length,
                                           size_t iterations,
                                           uint32_t desired_length)
{
  auto h = hmac::create(algorithm, password, password_length);
  const size_t hl = h->length();
  std::unique_ptr<char[]> work(new char[hl]);
  char* p = work.get();
  std::string rv;

  for (uint32_t counter = 1; desired_length; ++counter) {
    h->update(salt, salt_length);
    const uint32_t c = __crypto_be(counter);
    h->update((char*)&c, sizeof(c));

    auto bytes = h->finalize().bytes();
    memcpy(p, bytes.data(), bytes.length());

    for (size_t i = 1; i < iterations; ++i) {
      h->update(bytes);
      bytes = h->finalize().bytes();
      for (size_t j = 0; j < hl; ++j) {
        p[j] ^= bytes[j];
      }
    }
    auto use = std::min((size_t)desired_length, hl);
    rv.append(p, use);
    desired_length -= use;
  }
  return constant::Result(rv);
}
