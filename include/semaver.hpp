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
#include <charconv>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace semaver {

namespace detail {

namespace cmp {
constexpr int less = -1;
constexpr int equal = 0;
constexpr int greater = 1;
};

constexpr bool is_digits(const std::string_view str) noexcept {
  return !str.empty() && std::all_of(str.begin(), str.end(),
      [](const char c) { return '0' <= c && c <= '9'; });
}

inline std::vector<std::string_view> split(std::string_view str) noexcept {
  std::vector<std::string_view> output;
  while (true) {
    const auto pos = str.find('.');
    if (pos == str.npos) {
      output.emplace_back(str);
      break;
    }
    output.emplace_back(str.substr(0, pos));
    str.remove_prefix(pos + sizeof(char));
  }
  return output;
}

template <typename T>
constexpr T to_number(const std::string_view str) noexcept {
  T value{0};
  std::from_chars(str.data(), str.data() + str.size(), value, 10);
  return value;
}

}  // namespace detail

// Semantic Versioning 2.0.0 - https://semver.org
class Version {
public:
  using numeric_id_t = unsigned long;

  // Default version number is 0.1.0 (non-standard)
  Version(numeric_id_t major = 0,
          numeric_id_t minor = 1,
          numeric_id_t patch = 0,
          const std::string_view prerelease = std::string_view{},
          const std::string_view build = std::string_view{}) noexcept
      : major{major}, minor{minor}, patch{patch},
        prerelease{prerelease}, build{build} {}

  explicit Version(const std::string_view version) noexcept {
    parse(version);
  }

  Version& operator=(const Version& version) = default;

  std::string to_string() const noexcept {
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

  constexpr void increment_major(const numeric_id_t n = 1) noexcept {
    // Patch and minor version MUST be reset to 0 when major version is
    // incremented
    if (n) major += n; minor = 0; patch = 0;
  }
  constexpr void increment_minor(const numeric_id_t n = 1) noexcept {
    // Patch version MUST be reset to 0 when minor version is incremented
    if (n) minor += n; patch = 0;
  }
  constexpr void increment_patch(const numeric_id_t n = 1) noexcept {
    if (n) patch += n;
  }

  int compare(const Version& version) const noexcept {
    using namespace detail;

    // Major, minor, and patch versions are compared numerically
    if (major != version.major)
      return major < version.major ? cmp::less : cmp::greater;
    if (minor != version.minor)
      return minor < version.minor ? cmp::less : cmp::greater;
    if (patch != version.patch)
      return patch < version.patch ? cmp::less : cmp::greater;

    if (prerelease != version.prerelease) {
      // A pre-release version has lower precedence than a normal version
      if (version.prerelease.empty() || prerelease.empty())
        return version.prerelease.empty() ? cmp::less : cmp::greater;

      // Precedence for two pre-release versions MUST be determined by comparing
      // each dot separated identifier from left to right
      const auto lhs_ids = split(prerelease);
      const auto rhs_ids = split(version.prerelease);
      for (size_t i = 0; i < std::min(lhs_ids.size(), rhs_ids.size()); ++i) {
        const auto& lhs_id = lhs_ids.at(i);
        const auto& rhs_id = rhs_ids.at(i);
        const auto lhs_is_digits = is_digits(lhs_id);
        const auto rhs_is_digits = is_digits(rhs_id);

        // Identifiers consisting of only digits are compared numerically
        if (lhs_is_digits && rhs_is_digits) {
          const auto lhs_number = to_number<numeric_id_t>(lhs_id);
          const auto rhs_number = to_number<numeric_id_t>(rhs_id);
          if (lhs_number != rhs_number)
            return lhs_number < rhs_number ? cmp::less : cmp::greater;

        // Identifiers with letters or hyphens are compared lexically
        } else if (!lhs_is_digits && !rhs_is_digits) {
          const auto result = lhs_id.compare(rhs_id);
          if (result != 0)
            return result < 0 ? cmp::less : cmp::greater;

        // Numeric identifiers have lower precedence than non-numeric identifiers
        } else {
          return lhs_is_digits ? cmp::less : cmp::greater;
        }
      }

      // A larger set of pre-release fields has a higher precedence than a
      // smaller set
      if (lhs_ids.size() != rhs_ids.size())
        return lhs_ids.size() < rhs_ids.size() ? cmp::less : cmp::greater;
    }

    // Build metadata SHOULD be ignored when determining version precedence
    return cmp::equal;
  }

  numeric_id_t major = 0;
  numeric_id_t minor = 0;
  numeric_id_t patch = 0;
  std::string prerelease;
  std::string build;

private:
  bool parse(const std::string_view version) noexcept {
    // Reference: https://semver.org/#faq
    // License: CC BY 3.0
    // Added `v?` at the beginning for convenience.
    constexpr auto regex_pattern =
        "^v?(0|[1-9]\\d*)\\.(0|[1-9]\\d*)\\.(0|[1-9]\\d*)"
        "(?:-((?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*)"
        "(?:\\.(?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?"
        "(?:\\+([0-9a-zA-Z-]+(?:\\.[0-9a-zA-Z-]+)*))?$";

    static const std::regex regex{regex_pattern};
    std::cmatch match;

    if (!std::regex_match(version.data(), match, regex))
      return false;

    major = detail::to_number<numeric_id_t>(match[1].str());
    minor = detail::to_number<numeric_id_t>(match[2].str());
    patch = detail::to_number<numeric_id_t>(match[3].str());

    if (match[4].matched)
      prerelease = match[4].str();

    if (match[5].matched)
      build = match[5].str();

    return true;
  }
};

inline bool operator==(const Version& lhs, const Version& rhs) noexcept {
  return lhs.compare(rhs) == detail::cmp::equal;
}
inline bool operator!=(const Version& lhs, const Version& rhs) noexcept {
  return lhs.compare(rhs) != detail::cmp::equal;
}
inline bool operator< (const Version& lhs, const Version& rhs) noexcept {
  return lhs.compare(rhs) == detail::cmp::less;
}
inline bool operator> (const Version& lhs, const Version& rhs) noexcept {
  return lhs.compare(rhs) == detail::cmp::greater;
}
inline bool operator<=(const Version& lhs, const Version& rhs) noexcept {
  return lhs.compare(rhs) != detail::cmp::greater;
}
inline bool operator>=(const Version& lhs, const Version& rhs) noexcept {
  return lhs.compare(rhs) != detail::cmp::less;
}

}  // namespace semaver
