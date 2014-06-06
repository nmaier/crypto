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
#include <sstream>

#include "crypto_kdf.h"

using namespace std;
using namespace crypto::hash;
using namespace crypto::kdf;

/* Test arrays for HKDF. */
struct hkdfhash
{
  crypto::hash::Algorithms algorithm;
  size_t ikmlength;
  const char* ikm;
  size_t saltlength;
  const char* salt;
  size_t infolength;
  const char* info;
  size_t prklength;
  const char* prk;
  size_t okmlength;
  const char* okm;
} hkdfhashes[] = {
  {/* RFC 5869 A.1. Test Case 1 */
    algoSHA256,
    22,
    "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
    "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
    13,
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c",
    10,
    "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9",
    32,
    "077709362c2e32df0ddc3f0dc47bba6390b6c73bb50f9c3122ec844ad7c2b3e5",
    42,
    "3cb25f25faacd57a90434f64d0362f2a2d2d0a90cf1a5a4c5db02d56"
    "ecc4c5bf34007208d5b887185865"
  }, {/* RFC 5869 A.2. Test Case 2 */
    algoSHA256,
    80,
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d"
    "\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b"
    "\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29"
    "\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37"
    "\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43\x44\x45"
    "\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f",
    80,
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d"
    "\x6e\x6f\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b"
    "\x7c\x7d\x7e\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89"
    "\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97"
    "\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5"
    "\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf",
    80,
    "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd"
    "\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb"
    "\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9"
    "\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7"
    "\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5"
    "\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff",
    32,
    "06a6b88c5853361a06104c9ceb35b45c"
    "ef760014904671014a193f40c15fc244",
    82,
    "b11e398dc80327a1c8e7f78c596a4934"
    "4f012eda2d4efad8a050cc4c19afa97c"
    "59045a99cac7827271cb41c65e590e09"
    "da3275600c2f09b8367793a9aca3db71"
    "cc30c58179ec3e87c14c01d5c1f3434f"
    "1d87"
  }, {/* RFC 5869 A.3. Test Case 3 */
    algoSHA256,
    22,
    "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
    "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
    0,
    "",
    0,
    "",
    32,
    "19ef24a32c717b167f33a91d6f648bdf"
    "96596776afdb6377ac434c1c293ccb04",
    42,
    "8da4e775a563c18f715f802a063c5a31"
    "b8a11f5c5ee1879ec3454e5f3c738d2d"
    "9d201395faa4b61a96c8"
  }, {/* RFC 5869 A.4. Test Case 4 */
    algoSHA1,
    11,
    "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
    13,
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c",
    10,
    "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9",
    20,
    "9b6c18c432a7bf8f0e71c8eb88f4b30baa2ba243",
    42,
    "085a01ea1b10f36933068b56efa5ad81"
    "a4f14b822f5b091568a9cdd4f155fda2"
    "c22e422478d305f3f896"
  }, {/* RFC 5869 A.5. Test Case 5 */
    algoSHA1,
    80,
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d"
    "\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b"
    "\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29"
    "\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37"
    "\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43\x44\x45"
    "\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f",
    80,
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D"
    "\x6E\x6F\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B"
    "\x7C\x7D\x7E\x7F\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89"
    "\x8A\x8B\x8C\x8D\x8E\x8F\x90\x91\x92\x93\x94\x95\x96\x97"
    "\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xA0\xA1\xA2\xA3\xA4\xA5"
    "\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF",
    80,
    "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD"
    "\xBE\xBF\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB"
    "\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9"
    "\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7"
    "\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF0\xF1\xF2\xF3\xF4\xF5"
    "\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF",
    20,
    "8adae09a2a307059478d309b26c4115a224cfaf6",
    82,
    "0bd770a74d1160f7c9f12cd5912a06eb"
    "ff6adcae899d92191fe4305673ba2ffe"
    "8fa3f1a4e5ad79f3f334b3b202b2173c"
    "486ea37ce3d397ed034c7f9dfeb15c5e"
    "927336d0441f4c4300e2cff0d0900b52"
    "d3b4"
  }, {/* RFC 5869 A.6. Test Case 6 */
    algoSHA1,
    22,
    "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
    "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
    0,
    "",
    0,
    "",
    20,
    "da8c8a73c7fa77288ec6f5e7c297786aa0d32d01",
    42,
    "0ac1af7002b3d761d1e55298da9d0506"
    "b9ae52057220a306e07b6b87e8df21d0"
    "ea00033de03984d34918"
  }, {/* RFC 5869 A.7. Test Case 7. */
    algoSHA1,
    22,
    "\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"
    "\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c",
    0,
    0,
    0,
    "",
    20,
    "2adccada18779e7c2077ad2eb19d3f3e731385dd",
    42,
    "2c91117204d745f3500d636a62f64f0a"
    "b3bae548aa53d423b0d1f27ebba6f5e5"
    "673a081d70cce7acfc48"
  }
};

int test_hkdf()
{
  int rv = 0;
  for (const auto t : hkdfhashes) {
    auto r = HKDF(t.algorithm,
                  t.salt,
                  t.saltlength,
                  t.ikm,
                  t.ikmlength,
                  t.info,
                  t.infolength,
                  t.okmlength);
    stringstream ss;
    for (auto c : r.bytes()) {
      ss << setfill('0') << setw(2) << hex;
      ss << (int)(uint8_t)c;
    }
    if (ss.str() != t.okm) {
      cerr << "Expected: " << t.okm << "\nGot     : " << ss.str() << endl;
      rv = 1;
    }
    r = HKDF(t.algorithm,
             std::string(t.salt, t.saltlength),
             std::string(t.ikm, t.ikmlength),
             std::string(t.info, t.infolength),
             t.okmlength);
    ss.str("");
    for (auto c : r.bytes()) {
      ss << setfill('0') << setw(2) << hex;
      ss << (int)(uint8_t)c;
    }
    if (ss.str() != t.okm) {
      cerr << "Expected: " << t.okm << "\nGot     : " << ss.str() << endl;
      rv = 1;
    }
  }
  return rv;
}

struct
{
  const char* password;
  size_t passwordlength;
  const char* salt;
  size_t saltlength;
  size_t iterations;
  const char* derived;
  size_t derivedlength;
} pbkdf2s[] = {
  {
    "password",
    8,
    "salt",
    4,
    1,
    "0c60c80f961f0e71f3a9b524af6012062fe037a6",
    20
  }, {
    "password",
    8,
    "salt",
    4,
    2,
    "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957",
    20
  }, {
    "password",
    8,
    "salt",
    4,
    4096,
    "4b007901b765489abead49d926f721d065a429c1",
    20
  }, {
    "passwordPASSWORDpassword",
    24,
    "saltSALTsaltSALTsaltSALTsaltSALTsalt",
    36,
    4096,
    "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038",
    25
  }, {
    "pass\0word",
    9,
    "sa\0lt",
    5,
    4096,
    "56fa6aa75548099dcc37d7f03425e0c3",
    16
  }
};

int test_pbkdf2()
{
  int rv = 0;
  for (const auto t : pbkdf2s) {
    auto r = PBKDF2(algoSHA1,
                    t.password,
                    t.passwordlength,
                    t.salt,
                    t.saltlength,
                    t.iterations,
                    t.derivedlength);
    stringstream ss;
    for (auto c : r.bytes()) {
      ss << setfill('0') << setw(2) << hex;
      ss << (int)(uint8_t)c;
    }
    if (ss.str() != t.derived) {
      cerr << "Expected: " << t.derived << "\nGot     : " << ss.str() << endl;
      rv = 1;
    }
    r = PBKDF2(algoSHA1,
               std::string(t.password, t.passwordlength),
               std::string(t.salt, t.saltlength),
               t.iterations,
               t.derivedlength);
    ss.str("");
    for (auto c : r.bytes()) {
      ss << setfill('0') << setw(2) << hex;
      ss << (int)(uint8_t)c;
    }
    if (ss.str() != t.derived) {
      cerr << "Expected: " << t.derived << "\nGot     : " << ss.str() << endl;
      rv = 1;
    }
  }
  return rv;
}


int main(int argc, char* const argv[])
{
  int rv = 0;
  try {
    rv |= test_hkdf();
  }
  catch (const exception& ex) {
    cerr << "Error: " << ex.what() << endl;
    return 2;
  }
  return rv;
}
