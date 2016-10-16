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

#include <algorithm>
#include <vector>

#include "string.h"
#include "version.h"

namespace semaver {

SemanticVersion::SemanticVersion()
    : major(1), minor(0), patch(0) {
}

SemanticVersion::SemanticVersion(const string_t& version)
    : major(1), minor(0), patch(0) {
  Parse(version);
}

SemanticVersion::SemanticVersion(numeric_identifier_t major,
                                 numeric_identifier_t minor,
                                 numeric_identifier_t patch)
    : major(major), minor(minor), patch(patch) {
}

SemanticVersion& SemanticVersion::operator=(const SemanticVersion& version) {
  major = version.major;
  minor = version.minor;
  patch = version.patch;

  prerelease_identifiers = version.prerelease_identifiers;
  build_metadata = version.build_metadata;

  return *this;
}

SemanticVersion::operator string_t() const {
  string_t version = ToWstr(major) + L"." +
                     ToWstr(minor) + L"." +
                     ToWstr(patch);
  if (!prerelease_identifiers.empty())
    version += L"-" + prerelease_identifiers;
  if (!build_metadata.empty())
    version += L"+" + build_metadata;

  return version;
}

////////////////////////////////////////////////////////////////////////////////

CompareResult SemanticVersion::Compare(const SemanticVersion& version) const {
  if (major != version.major)
    return major < version.major ? kLessThan : kGreaterThan;
  if (minor != version.minor)
    return minor < version.minor ? kLessThan : kGreaterThan;
  if (patch != version.patch)
    return patch < version.patch ? kLessThan : kGreaterThan;

  if (prerelease_identifiers != version.prerelease_identifiers) {
    if (prerelease_identifiers.empty() &&
        !version.prerelease_identifiers.empty())
      return kGreaterThan;
    if (!prerelease_identifiers.empty() &&
        version.prerelease_identifiers.empty())
      return kLessThan;

    std::vector<string_t> identifiers_, identifiers;
    Split(prerelease_identifiers, L".", identifiers_);
    Split(version.prerelease_identifiers, L".", identifiers);

    size_t min_size = std::min(identifiers_.size(), identifiers.size());
    for (size_t i = 0; i < min_size; ++i) {
      if (IsNumericString(identifiers_.at(i)) &&
          IsNumericString(identifiers.at(i))) {
        int lhs = ToInt(identifiers_.at(i));
        int rhs = ToInt(identifiers.at(i));
        if (lhs != rhs)
          return lhs < rhs ? kLessThan : kGreaterThan;
      } else {
        int result = CompareStrings(identifiers_.at(i), identifiers.at(i), false, 260);
        if (result != 0)
          return result < 0 ? kLessThan : kGreaterThan;
      }
    }

    if (identifiers_.size() != identifiers.size())
      return identifiers_.size() < identifiers.size() ?
          kLessThan : kGreaterThan;
  }

  return kEqualTo;
}

void SemanticVersion::Parse(const string_t& version) {
  std::vector<string_t> identifiers;
  std::wstring last_one;

  Split(version, L".", identifiers);

  if (identifiers.empty())
    return;

  last_one = identifiers.back();
  identifiers.pop_back();
  Split(last_one, L"-", identifiers);

  last_one = identifiers.back();
  identifiers.pop_back();
  Split(last_one, L"+", identifiers);

  if (identifiers.size() > 0)
    major = ToInt(identifiers.at(0));
  if (identifiers.size() > 1)
    minor = ToInt(identifiers.at(1));
  if (identifiers.size() > 2)
    patch = ToInt(identifiers.at(2));
  if (identifiers.size() > 3)
    prerelease_identifiers = identifiers.at(3);
  if (identifiers.size() > 4)
    build_metadata = identifiers.at(4);
}

}  // namespace semaver
