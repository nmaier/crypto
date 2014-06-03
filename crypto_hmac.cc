/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#include "crypto_hmac.h"

using namespace crypto::hmac;

HMAC::HMAC(hash::Algorithms algorithm, const char* secret, size_t length)
  : hash_(hash::create(algorithm)),
    blockSize_(hash_->blocksize()),
    clean_(false)
{
  ipad_.assign(blockSize_, 0x36);
  opad_.assign(blockSize_, 0x5c);

  if (length > blockSize_) {
    hash_->update(secret, length);
    auto hash = hash_->finalize();
    for (size_t i = 0uL, e = hash.length(); i < e; ++i) {
      ipad_.replace(i, 1, 1, hash[i] ^ 0x36);
      opad_.replace(i, 1, 1, hash[i] ^ 0x5c);
    }
  }
  else {
    for (size_t i = 0uL, e = length; i < e; ++i) {
      ipad_.replace(i, 1, 1, secret[i] ^ 0x36);
      opad_.replace(i, 1, 1, secret[i] ^ 0x5c);
    }
  }
  reset();
}

#if 0
std::unique_ptr<HMAC> ::createRandom(const std::string& algorithm)
{
  const auto len = MessageDigest::getDigestLength(algorithm);
  if (len == 0) {
    return nullptr;
  }
  auto buf = make_unique<char[]>(len);
  generateRandomData((unsigned char*)buf.get(), len);
  return create(algorithm, buf.get(), len);
}
#endif
