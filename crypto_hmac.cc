/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#include "crypto_hmac.h"
#include "crypto_rnd.h"

using namespace crypto;
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

std::unique_ptr<HMAC> crypto::hmac::createRandom(hash::Algorithms algorithm)
{
  auto a = create(algorithm);
  const auto len = a->blocksize();
  if (len == 0) {
    return nullptr;
  }
  auto buf = std::unique_ptr<uint8_t[]>(new uint8_t[len]);
  rnd::randomBytes(buf.get(), len);
  return create(algorithm, (char*)buf.get(), len);
}
