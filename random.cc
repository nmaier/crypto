/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */
// Written in 2014 by Nils Maier

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <cmath>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "crypto_rnd.h"

using namespace std;

int main(int argc, char* const argv[])
{
  try
  {
    map<uint8_t, size_t> counts;
    uint8_t bytes[1 << 20];
    for (auto i = 0; i < 10; ++i) {
      crypto::rnd::randomBytes(bytes, sizeof(bytes));
      for (auto b : bytes) {
        counts[b]++;
      }
    }
    if (counts.size() != 256) {
      throw std::domain_error(
          "Would have expected to see at one of each possible byte value!");
    }
    double sum =
        accumulate(counts.begin(),
                   counts.end(),
                   0.0,
                   [](double total, const decltype(counts)::value_type & elem) {
          return total + elem.second;
        });
    double mean = sum / counts.size();

    vector<double> diff(counts.size());
    transform(counts.begin(),
              counts.end(),
              diff.begin(),
              [&](const decltype(counts)::value_type & elem)
                  -> double { return (double)elem.second - mean; });
    double sq_sum = inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    double stddev = sqrt(sq_sum / counts.size());
    cout << "stddev: " << fixed << stddev << endl;
    if (stddev > 320) {
      throw std::domain_error("stdev > 320");
    }
    if (stddev < 100) {
      throw std::domain_error("stdev < 100");
    }
    return 0;
  }
  catch (std::exception& ex)
  {
    cerr << "Error: " << ex.what() << endl;
    return 2;
  }
}
