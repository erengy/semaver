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

#ifndef TAIGA_BASE_VERSION_H
#define TAIGA_BASE_VERSION_H

#include <string>

#include "comparable.h"

namespace base {

// An implementation of Semantic Versioning 2.0.0 that provides an easy way
// to compare version numbers.
//
// See http://semver.org for Semantic Versioning Specification.

class SemanticVersion : public Comparable<SemanticVersion> {
public:
  typedef unsigned int numeric_identifier_t;
  typedef std::wstring string_t;

  enum Version {
    kMajor,
    kMinor,
    kPatch,
    kPreRelease,
    kBuildMetadata
  };

  SemanticVersion();
  SemanticVersion(const string_t& version);
  SemanticVersion(numeric_identifier_t major,
                  numeric_identifier_t minor,
                  numeric_identifier_t patch);
  ~SemanticVersion() {}

  SemanticVersion& operator = (const SemanticVersion& version);

  operator string_t() const;

  numeric_identifier_t major;
  numeric_identifier_t minor;
  numeric_identifier_t patch;
  string_t prerelease_identifiers;
  string_t build_metadata;

private:
  CompareResult Compare(const SemanticVersion& version) const;
  void Parse(const string_t& version);
};

}  // namespace base

#endif  // TAIGA_BASE_VERSION_H