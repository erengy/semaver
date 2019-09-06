/*
MIT License

Copyright (c) 2016-2019 Eren Okka

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

#include <algorithm>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace semaver {

namespace detail {

enum CompareResult {
  kLessThan = -1,
  kEqualTo = 0,
  kGreaterThan = 1,
};

constexpr auto regex_pattern{
  "(0|[1-9][0-9]*)\\."
  "(0|[1-9][0-9]*)\\."
  "(0|[1-9][0-9]*)"
  "(?:\\-([0-9A-Za-z\\-]+(?:\\.[0-9A-Za-z\\-]+)*))?"
  "(?:\\+([0-9A-Za-z\\-]+(?:\\.[0-9A-Za-z\\-]+)*))?"
};

constexpr bool IsDigits(const std::string_view str) {
  return !str.empty() && std::all_of(str.begin(), str.end(),
      [](const char c) {
        return '0' <= c && c <= '9';
      });
};

inline std::vector<std::string_view> Split(std::string_view str) {
  std::vector<std::string_view> output;

  while (true) {
    const size_t pos = str.find('.');
    if (pos == str.npos) {
      output.emplace_back(str);
      break;
    }
    output.emplace_back(str.substr(0, pos));
    str.remove_prefix(pos + sizeof(char));
  }

  return output;
};

}  // namespace detail

// Semantic Versioning 2.0.0 - http://semver.org
class Version {
public:
  enum NumericIdentifier {
    kMajor,
    kMinor,
    kPatch,
  };

  // Default version number is 0.1.0 (non-standard)
  Version(unsigned long major = 0,
          unsigned long minor = 1,
          unsigned long patch = 0,
          const std::string_view prerelease = std::string_view{},
          const std::string_view build = std::string_view{})
      : major{major}, minor{minor}, patch{patch},
        prerelease{prerelease}, build{build} {}

  explicit Version(const std::string_view version) {
    Parse(version);
  }

  Version& operator=(const Version& version) = default;

  operator std::string() const {
    return to_string();
  }

  std::string to_string() const {
    // A normal version number MUST take the form X.Y.Z
    std::string version = std::to_string(major) + '.' +
                          std::to_string(minor) + '.' +
                          std::to_string(patch);

    // A pre-release version MAY be denoted by appending a hyphen
    if (!prerelease.empty())
      version += '-' + prerelease;

    // Build metadata MAY be denoted by appending a plus sign
    if (!build.empty())
      version += '+' + build;

    return version;
  }

  // Increments given numeric identifier by given number, and resets lesser
  // identifiers to 0.
  constexpr void Increment(NumericIdentifier id, unsigned long n = 1) {
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

  int Compare(const Version& version) const {
    using namespace detail;

    // Major, minor, and patch versions are compared numerically
    if (major != version.major)
      return major < version.major ? kLessThan : kGreaterThan;
    if (minor != version.minor)
      return minor < version.minor ? kLessThan : kGreaterThan;
    if (patch != version.patch)
      return patch < version.patch ? kLessThan : kGreaterThan;

    if (prerelease != version.prerelease) {
      // A pre-release version has lower precedence than a normal version
      if (version.prerelease.empty() || prerelease.empty())
        return version.prerelease.empty() ? kLessThan : kGreaterThan;

      // Precedence for two pre-release versions MUST be determined by comparing
      // each dot separated identifier from left to right
      const auto lhs_ids = Split(prerelease);
      const auto rhs_ids = Split(version.prerelease);
      for (size_t i = 0; i < std::min(lhs_ids.size(), rhs_ids.size()); ++i) {
        const auto& lhs_id = lhs_ids.at(i);
        const auto& rhs_id = rhs_ids.at(i);
        const auto lhs_is_digits = IsDigits(lhs_id);
        const auto rhs_is_digits = IsDigits(rhs_id);

        // Identifiers consisting of only digits are compared numerically
        if (lhs_is_digits && rhs_is_digits) {
          const auto lhs_number = std::stoul(lhs_id.data());
          const auto rhs_number = std::stoul(rhs_id.data());
          if (lhs_number != rhs_number)
            return lhs_number < rhs_number ? kLessThan : kGreaterThan;

        // Identifiers with letters or hyphens are compared lexically
        } else if (!lhs_is_digits && !rhs_is_digits) {
          const auto result = lhs_id.compare(rhs_id);
          if (result != 0)
            return result < 0 ? kLessThan : kGreaterThan;

        // Numeric identifiers have lower precedence than non-numeric identifiers
        } else {
          return lhs_is_digits ? kLessThan : kGreaterThan;
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

  unsigned long major = 0;
  unsigned long minor = 0;
  unsigned long patch = 0;

  std::string prerelease;
  std::string build;

private:
  bool Parse(const std::string_view version) {
    static const std::regex regex{detail::regex_pattern};
    std::cmatch match;

    if (!std::regex_match(version.data(), match, regex))
      return false;

    major = std::stoul(match[1].str());
    minor = std::stoul(match[2].str());
    patch = std::stoul(match[3].str());

    if (match[4].matched)
      prerelease = match[4].str();

    if (match[5].matched)
      build = match[5].str();

    return true;
  }
};

inline bool operator==(const Version& lhs, const Version& rhs) {
  return lhs.Compare(rhs) == detail::kEqualTo;
}
inline bool operator!=(const Version& lhs, const Version& rhs) {
  return lhs.Compare(rhs) != detail::kEqualTo;
}
inline bool operator< (const Version& lhs, const Version& rhs) {
  return lhs.Compare(rhs) == detail::kLessThan;
}
inline bool operator> (const Version& lhs, const Version& rhs) {
  return lhs.Compare(rhs) == detail::kGreaterThan;
}
inline bool operator<=(const Version& lhs, const Version& rhs) {
  return lhs.Compare(rhs) != detail::kGreaterThan;
}
inline bool operator>=(const Version& lhs, const Version& rhs) {
  return lhs.Compare(rhs) != detail::kLessThan;
}

}  // namespace semaver
