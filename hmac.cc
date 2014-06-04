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

#include <fcntl.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#define open _open
#define close _close
#define read _read
#define fileno _fileno
#endif

#ifndef _WIN32
#include <sys/mman.h>
#endif // _WIN32

#include "crypto_hmac.h"

using namespace std;

static char buffer[1<<18];

template <typename H>
static void print_hmac(H& h, const string& file, const string& secret)
{
  int fd = fileno(stdin);
#ifdef _WIN32
  _setmode(fd, O_BINARY);
#endif
  if (file != "-") {
#ifndef _WIN32
    fd = open(file.c_str(), O_RDONLY);
#else // _WIN32
    fd = open(file.c_str(), O_RDONLY | O_BINARY);
#endif // _WIN32
  }
  if (fd < 0) {
    cerr << "Error: Failed to open file " << file << endl;
    return;
  }

  for (;;) {
    auto r = read(fd, buffer, sizeof(buffer));
    if (r == 0) {
      break;
    }
    if (r < 0) {
      cerr << "Error: Failed to read from file " << file << endl;
      goto out;
    }
#ifndef _WIN32
    madvise(buffer, r, MADV_SEQUENTIAL | MADV_WILLNEED);
#endif // _WIN32
    h->update(buffer, r);
  }

  {
    auto r = h->finalize();
    cout << setfill('0') << setw(2);
    for (auto c : r.bytes()) {
      cout << setw(2) << hex << (int)(uint8_t)c;
    }
    cout << "  (" << secret << ") " << file << endl << flush;
  }

out:
  if (file != "-") {
    close(fd);
  }
}

static void usage(const char* prog)
{
  cout << prog << " [-a <algorithm>] [-s <secret>] [file ...]" << endl;
  cout << endl;
  cout << "Create a hmac for given files using the specified secret." << endl;
  cout << endl;
  cout << "If no files are given, " << prog
       << " will read the input from stdin." << endl;
  cout << "If no algorithm is specified, then 'sha' is chosen by default."
       << endl;
  cout << "If no secret is specified, secret will be an empty string." << endl;
  cout << endl;
  cout << "Available algorithms:" << endl;
  for (auto& i : crypto::hash::all()) {
    cout << "  - " << i << endl;
  }
}

#ifdef _MSC_VER
// Most brilliant getopt implementation ever!!!
namespace {
static const char *optarg;
static const char *next = nullptr;
static int optind = 0;

static int getopt(int argc, const char * const argv[], const char *optstring)
{
  if (!optind) {
    next = nullptr;
  }
  optarg = nullptr;

  if (!next || !*next) {
    if (optind == 0) {
      optind++;
    }

    if (optind >= argc || argv[optind][0] != '-' || !argv[optind][1]) {
      optarg = nullptr;
      if (optind < argc) {
        optarg = argv[optind];
      }
      return -1;
    }

    if (!strcmp(argv[optind], "--")) {
      optind++;
      optarg = nullptr;
      if (optind < argc) {
        optarg = argv[optind];
      }
      return -1;
    }

    next = argv[optind];
    next++;
    optind++;
  }

  const char c = *next++;
  const char *cp = strchr(optstring, c);

  if (!cp || c == ':') {
    return '?';
  }

  cp++;
  if (*cp == ':') {
    if (*next) {
      optarg = next;
      next = nullptr;
    }
    else if (optind < argc) {
      optarg = argv[optind];
      optind++;
    }
    else {
      return '?';
    }
  }

  return c;
}
} // namespace
#endif

int main(int argc, char* const argv[])
{
  auto algo = "sha";
  auto secret = "";
  int ch;
  while ((ch = getopt(argc, argv, "a:s:h?")) != -1) {
    switch (ch) {
    case 'a':
      algo = optarg;
      break;

    case 's':
      secret = optarg;
      break;

    case 'h':
    case '?':
    default:
      usage(*argv);
      return 0;
    }
  }

  argc -= optind;
  argv += optind;

  try
  {
    auto ctx = strcmp(secret, "rnd") ? crypto::hmac::create(algo, secret) :
                                       crypto::hmac::createRandom(algo);
    cout << setfill('0') << setw(2);
    if (argc == 0) {
      print_hmac(ctx, "-", secret);
      return 0;
    }
    for (auto i = 0; i < argc; ++i) {
      print_hmac(ctx, argv[i], secret);
    }
  }
  catch (const std::exception& ex)
  {
    cerr << "Error: " << ex.what() << endl;
    return 1;
  }
  return 0;
}
