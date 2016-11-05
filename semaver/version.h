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

#pragma once

#include <string>
#include <vector>

#include "comparable.h"

namespace semaver {

const std::string regex_pattern{
  "(0|[1-9][0-9]*)\\."
  "(0|[1-9][0-9]*)\\."
  "(0|[1-9][0-9]*)"
  "(?:\\-([0-9A-Za-z\\-]+(?:\\.[0-9A-Za-z\\-]+)*))?"
  "(?:\\+([0-9A-Za-z\\-]+(?:\\.[0-9A-Za-z\\-]+)*))?"
};

// Semantic Versioning 2.0.0 - http://semver.org
class SemanticVersion : public Comparable<SemanticVersion> {
public:
  enum NumericIdentifier {
    kMajor,
    kMinor,
    kPatch,
  };

  SemanticVersion() {}
  SemanticVersion(unsigned long major, unsigned long minor, unsigned long patch);
  SemanticVersion(const std::string& version);
  ~SemanticVersion() {}

  SemanticVersion& operator=(const SemanticVersion& version);

  operator std::string() const;
  std::string str() const;

  // Increments given numeric identifier by given number, and resets lesser
  // identifiers to 0.
  void Increment(NumericIdentifier identifier, unsigned long n = 1);

  unsigned long major = 0;
  unsigned long minor = 1;
  unsigned long patch = 0;

  std::string prerelease_identifiers;
  std::string build_metadata;

private:
  CompareResult Compare(const SemanticVersion& version) const;
  bool Parse(const std::string& version);
};

}  // namespace semaver
