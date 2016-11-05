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
#include <regex>
#include <vector>

#include "string.h"
#include "version.h"

namespace semaver {

Version::Version(unsigned long major, unsigned long minor, unsigned long patch)
    : major(major), minor(minor), patch(patch) {
}

Version::Version(const std::string& version) {
  Parse(version);
}

Version& Version::operator=(const Version& version) {
  major = version.major;
  minor = version.minor;
  patch = version.patch;

  prerelease_identifiers = version.prerelease_identifiers;
  build_metadata = version.build_metadata;

  return *this;
}

Version::operator std::string() const {
  return str();
}

std::string Version::str() const {
  // A normal version number MUST take the form X.Y.Z
  std::string version = NumberToString(major) + "." +
                        NumberToString(minor) + "." +
                        NumberToString(patch);

  // A pre-release version MAY be denoted by appending a hyphen
  if (!prerelease_identifiers.empty())
    version += "-" + prerelease_identifiers;

  // Build metadata MAY be denoted by appending a plus sign
  if (!build_metadata.empty())
    version += "+" + build_metadata;

  return version;
}

void Version::Increment(NumericIdentifier id, unsigned long n) {
  if (n == 0)
    return;  // to avoid invalid resets

  switch (id) {
    // Patch and minor version MUST be reset to 0 when major version is
    // incremented
    case kMajor:
      major += n; minor = 0; patch = 0;
      break;

    // Patch version MUST be reset to 0 when minor version is incremented
    case kMinor:
      minor += n; patch = 0;
      break;

    case kPatch:
      patch += n;
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////

CompareResult Version::Compare(const Version& version) const {
  // Major, minor, and patch versions are compared numerically
  if (major != version.major)
    return major < version.major ? kLessThan : kGreaterThan;
  if (minor != version.minor)
    return minor < version.minor ? kLessThan : kGreaterThan;
  if (patch != version.patch)
    return patch < version.patch ? kLessThan : kGreaterThan;

  if (prerelease_identifiers != version.prerelease_identifiers) {
    // A pre-release version has lower precedence than a normal version
    if (!prerelease_identifiers.empty()) {
      if (version.prerelease_identifiers.empty())
        return kLessThan;
    } else {
      if (!version.prerelease_identifiers.empty())
        return kGreaterThan;
    }

    // Precedence for two pre-release versions MUST be determined by comparing
    // each dot separated identifier from left to right
    const auto lhs_ids = SplitString(prerelease_identifiers, ".");
    const auto rhs_ids = SplitString(version.prerelease_identifiers, ".");

    for (size_t i = 0; i < std::min(lhs_ids.size(), rhs_ids.size()); ++i) {
      const auto& lhs = lhs_ids.at(i);
      const auto& rhs = rhs_ids.at(i);

      const bool lhs_is_numeric = IsNumericString(lhs);
      const bool rhs_is_numeric = IsNumericString(rhs);

      // Identifiers consisting of only digits are compared numerically
      if (lhs_is_numeric && rhs_is_numeric) {
        const auto lhs_number = StringToNumber(lhs);
        const auto rhs_number = StringToNumber(rhs);
        if (lhs_number != rhs_number)
          return lhs_number < rhs_number ? kLessThan : kGreaterThan;

      // Identifiers with letters or hyphens are compared lexically
      } else if (!lhs_is_numeric && !rhs_is_numeric) {
        const auto result = CompareStrings(lhs, rhs);
        if (result != 0)
          return result < 0 ? kLessThan : kGreaterThan;

      // Numeric identifiers have lower precedence than non-numeric identifiers
      } else {
        return lhs_is_numeric ? kLessThan : kGreaterThan;
      }
    }

    // A larger set of pre-release fields has a higher precedence than a
    // smaller set
    if (lhs_ids.size() != rhs_ids.size())
      return lhs_ids.size() < rhs_ids.size() ? kLessThan : kGreaterThan;
  }

  // Build metadata SHOULD be ignored when determining version precedence
  return kEqualTo;
}

bool Version::Parse(const std::string& version) {
  static const auto regex = std::regex(regex_pattern);
  std::smatch match;

  if (!std::regex_match(version, match, regex))
    return false;

  major = StringToNumber(match[1].str());
  minor = StringToNumber(match[2].str());
  patch = StringToNumber(match[3].str());

  if (match[4].matched)
    prerelease_identifiers = match[4].str();

  if (match[5].matched)
    build_metadata = match[5].str();

  return true;
}

}  // namespace semaver
