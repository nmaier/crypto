/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#ifndef CRYPTO_HMAC_H
#define CRYPTO_HMAC_H

#include <string>
#include <stdexcept>

#include "crypto_constant.h"
#include "crypto_hash.h"

namespace crypto {
namespace hmac {

/**
 * Implements HMAC-SHA* per RFC 6234. It supports the same cryptographic hash
 * algorithms that MessageDigest supports, but at most the SHA-1, SHA-2
 * algorithms as specified in the RFC.
 */
class HMAC
{
public:
  /**
   * Constructs a new HMAC. It is recommended to use the |create| or
   * |createRandom| factory methods instead.
   *
   * @see create
   * @see createRandom
   */
  HMAC(hash::Algorithms algorithm, const char* secret, size_t length);

  /**
   * Tells the length in bytes of the resulting HMAC.
   */
  size_t length() const
  {
    return hash_->length();
  }

  /**
   * Resets the instance, clearing the internal state. The instance can be
   * re-used afterwards.
   */
  void reset()
  {
    if (clean_) {
      return;
    }
    hash_->reset();
    hash_->update(ipad_.data(), ipad_.length());
    clean_ = true;
  }

  /**
   * Updates the HMAC with new message data.
   */
  void update(const std::string& data)
  {
    hash_->update(data.data(), data.length());
    clean_ = false;
  }

  /**
   * Updates the HMAC with new message data.
   */
  void update(const char* data, size_t length)
  {
    hash_->update(data, length);
    clean_ = false;
  }

  /**
   * Returns the result. This can only be called once. After the call the
   * internal state is reset and new HMACs can be computed with the same
   * instance.
   */
  constant::Result finalize()
  {
    auto rv = hash_->finalize();
    hash_->update(opad_.data(), opad_.length());
    hash_->update(rv);
    rv = hash_->finalize();
    clean_ = false;
    reset();
    return rv;
  }

  /**
   * Returns the resulting HMAC of string in one go. You cannot mix call to this
   * method with calls to update.
   */
  constant::Result finalize(const std::string& str)
  {
    reset();
    update(str);
    return finalize();
  }

  /**
   * Returns the resulting HMAC of string in one go. You cannot mix call to this
   * method with calls to update.
   */
  constant::Result finalize(const char* data, size_t len)
  {
    reset();
    update(data, len);
    return finalize();
  }

private:
  std::unique_ptr<hash::Algorithm> hash_;
  const size_t blockSize_;
  std::string ipad_, opad_;
  bool clean_;
};

/**
 * Creates a new instance using the specified algorithm and secret.
 */
inline std::unique_ptr<HMAC>
create(hash::Algorithms algorithm, const char* secret, size_t length)
{
  return std::unique_ptr<HMAC>(new HMAC(algorithm, secret, length));
}

inline std::unique_ptr<HMAC> create(hash::Algorithms algorithm,
                                    const std::string& secret)
{
  return create(algorithm, secret.data(), secret.length());
}

inline std::unique_ptr<HMAC>
create(const std::string& algorithm, const char* secret, size_t length)
{
  return create(hash::lookup(algorithm), secret, length);
}

inline std::unique_ptr<HMAC> create(const std::string& algorithm,
                                    const std::string& secret)
{
  return create(hash::lookup(algorithm), secret.data(), secret.length());
}

/**
 * Creates a new instance using sha-1 and the specified secret.
 */
inline std::unique_ptr<HMAC> create(const char* secret, size_t length)
{
  return create(hash::algoSHA1, secret, length);
}

inline std::unique_ptr<HMAC> create(const std::string& secret)
{
  return create(hash::algoSHA1, secret.data(), secret.length());
}

/**
 * Creates a new instance using the specified algorithm and a random secret.
 */
std::unique_ptr<HMAC> createRandom(const hash::Algorithms algorithm);

inline std::unique_ptr<HMAC> createRandom(const std::string& algorithm)
{
  return createRandom(hash::lookup(algorithm));
}

/**
 * Creates a new instance using sha-1 and a random secret.
 */
inline std::unique_ptr<HMAC> createRandom()
{
  return createRandom(hash::algoSHA1);
}

} // namespace hmac
} // namespace crypto

#endif // CRYPTO_HMAC_H
