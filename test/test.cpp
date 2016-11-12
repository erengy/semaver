/*
MIT License

Copyright (c) 2016 Eren Okka

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <cassert>
#include <iostream>
#include <limits>

#include "../semaver/version.h"

namespace {

using sv = semaver::Version;

constexpr auto numeric_min = std::numeric_limits<unsigned long>::min();
constexpr auto numeric_max = std::numeric_limits<unsigned long>::max();

////////////////////////////////////////////////////////////////////////////////

void test_initialize() {
  assert(sv() == sv(0, 1, 0));

  assert(sv(sv(1, 2, 3)) == sv(1, 2, 3));

  assert(sv(numeric_min, numeric_min, numeric_min) == sv(0, 0, 0));

  assert(sv(numeric_max, numeric_max, numeric_max) ==
         sv(numeric_max, numeric_max, numeric_max));
}

////////////////////////////////////////////////////////////////////////////////

void test_assign() {
  auto v0 = sv(0, 0, 0);

  auto v1 = sv(1, 1, 1);
  v1.prerelease_identifiers = "a";
  v1.build_metadata = "a";

  v0 = v1;
  assert(v0 == v1);
}

////////////////////////////////////////////////////////////////////////////////

void test_compare_numeric() {
  assert(sv(0, 0, 1) == sv(0, 0, 1));
  assert(sv(0, 1, 0) == sv(0, 1, 0));
  assert(sv(1, 0, 0) == sv(1, 0, 0));

  assert(sv(0, 0, 0) != sv(0, 0, 1));
  assert(sv(0, 0, 0) != sv(0, 1, 0));
  assert(sv(0, 0, 0) != sv(1, 0, 0));

  assert(sv(0, 0, 0) < sv(0, 0, 1));
  assert(sv(0, 0, 0) < sv(0, 1, 0));
  assert(sv(0, 0, 0) < sv(1, 0, 0));

  assert(sv(0, 0, 0) <= sv(0, 0, 1));
  assert(sv(0, 0, 0) <= sv(0, 1, 0));
  assert(sv(0, 0, 0) <= sv(1, 0, 0));

  assert(sv(0, 0, 1) > sv(0, 0, 0));
  assert(sv(0, 1, 0) > sv(0, 0, 0));
  assert(sv(1, 0, 0) > sv(0, 0, 0));

  assert(sv(0, 0, 1) >= sv(0, 0, 0));
  assert(sv(0, 1, 0) >= sv(0, 0, 0));
  assert(sv(1, 0, 0) >= sv(0, 0, 0));
}

void test_compare_prerelease() {
  auto v0 = sv();
  auto v1 = sv();

  v0.prerelease_identifiers = "";
  v1.prerelease_identifiers = "a";
  assert(v0 > v1);

  v0.prerelease_identifiers = "a";
  v1.prerelease_identifiers = "a";
  assert(v0 == v1);

  v0.prerelease_identifiers = "0";
  v1.prerelease_identifiers = "1";
  assert(v0 < v1);

  v0.prerelease_identifiers = "a";
  v1.prerelease_identifiers = "b";
  assert(v0 < v1);

  v0.prerelease_identifiers = "0";
  v1.prerelease_identifiers = "a";
  assert(v0 < v1);

  v0.prerelease_identifiers = "0.a.0a";
  v1.prerelease_identifiers = "0.a.0a";
  assert(v0 == v1);

  v0.prerelease_identifiers = "0.a.0a";
  v1.prerelease_identifiers = "1.a.0a";
  assert(v0 < v1);

  v0.prerelease_identifiers = "0.a.0a";
  v1.prerelease_identifiers = "0.b.0a";
  assert(v0 < v1);

  v0.prerelease_identifiers = "0.a.0a";
  v1.prerelease_identifiers = "0.a.0b";
  assert(v0 < v1);
}

void test_compare_build() {
  auto v0 = sv();
  auto v1 = sv();

  v0.build_metadata = "abc";
  v1.build_metadata = "xyz";
  assert(v0 == v1);
}

////////////////////////////////////////////////////////////////////////////////

void test_increment_single() {
  auto v = sv(0, 0, 0);

  v.Increment(sv::kPatch);
  assert(v == sv(0, 0, 1));

  v.Increment(sv::kMinor);
  assert(v == sv(0, 1, 0));

  v.Increment(sv::kPatch);
  assert(v == sv(0, 1, 1));

  v.Increment(sv::kMajor);
  assert(v == sv(1, 0, 0));
}

void test_increment_multi() {
  auto v = sv(0, 0, 0);

  v.Increment(sv::kPatch, 2);
  assert(v == sv(0, 0, 2));

  v.Increment(sv::kMinor, 3);
  assert(v == sv(0, 3, 0));

  v.Increment(sv::kPatch, 4);
  assert(v == sv(0, 3, 4));

  v.Increment(sv::kMajor, 5);
  assert(v == sv(5, 0, 0));
}

void test_increment_limits() {
  auto v = sv(0, 0, 0);

  v.Increment(sv::kMajor, numeric_min);
  v.Increment(sv::kMinor, numeric_min);
  v.Increment(sv::kPatch, numeric_min);
  assert(v == sv(0, 0, 0));

  v.Increment(sv::kMajor, numeric_max);
  v.Increment(sv::kMinor, numeric_max);
  v.Increment(sv::kPatch, numeric_max);
  assert(v == sv(numeric_max, numeric_max, numeric_max));

  v.Increment(sv::kPatch, 1);
  assert(v == sv(numeric_max, numeric_max, 0));
  v.Increment(sv::kMinor, 1);
  assert(v == sv(numeric_max, 0, 0));
  v.Increment(sv::kMajor, 1);
  assert(v == sv(0, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////

void test_serialize() {
  assert(sv(0, 0, 1).str() == "0.0.1");
  assert(sv(0, 1, 0).str() == "0.1.0");
  assert(sv(1, 0, 0).str() == "1.0.0");

  auto v = sv(1, 0, 0);

  v.prerelease_identifiers = "a";
  v.build_metadata = "";
  assert(v.str() == "1.0.0-a");

  v.prerelease_identifiers = "";
  v.build_metadata = "b";
  assert(v.str() == "1.0.0+b");

  v.prerelease_identifiers = "a";
  v.build_metadata = "b";
  assert(v.str() == "1.0.0-a+b");
}

void test_parse() {
  auto v = sv(1, 2, 3);

  assert(v == sv("1.2.3"));

  v.build_metadata = "";
  v.prerelease_identifiers = "a";
  assert(v == sv("1.2.3-a"));
  v.prerelease_identifiers = "a.b";
  assert(v == sv("1.2.3-a.b"));
  v.prerelease_identifiers = "a.b.c";
  assert(v == sv("1.2.3-a.b.c"));

  v.prerelease_identifiers = "";
  v.build_metadata = "x";
  assert(v == sv("1.2.3+x"));
  v.build_metadata = "x.y";
  assert(v == sv("1.2.3+x.y"));
  v.build_metadata = "x.y.z";
  assert(v == sv("1.2.3+x.y.z"));

  v.prerelease_identifiers = "a";
  v.build_metadata = "x";
  assert(v == sv("1.2.3-a+x"));
  v.prerelease_identifiers = "a.b";
  v.build_metadata = "x.y";
  assert(v == sv("1.2.3-a.b+x.y"));
  v.prerelease_identifiers = "a.b.c";
  v.build_metadata = "x.y.z";
  assert(v == sv("1.2.3-a.b.c+x.y.z"));
}

////////////////////////////////////////////////////////////////////////////////

void test_all() {
  test_initialize();

  test_assign();

  test_compare_numeric();
  test_compare_prerelease();
  test_compare_build();

  test_increment_single();
  test_increment_multi();
  test_increment_limits();

  test_serialize();
  test_parse();

  std::cout << "Passed all tests!\n";
}

}  // namespace

////////////////////////////////////////////////////////////////////////////////

int main() {
  test_all();
  return 0;
}