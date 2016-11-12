# Semaver

*Semaver* is a C++ library for using [Semantic Versioning](http://semver.org) in your applications.

## Example

```cpp
#include <iostream>
#include <semaver/version.h>

using SemVer = semaver::Version;

int main() {
  auto v0 = SemVer();  // 0.1.0 by default
  auto v1 = SemVer(1, 2, 3);
  auto v2 = SemVer("2.0.0");
  auto v3 = SemVer("3.0.0-beta+2016.10.16");

  v0 = v1;  // 0.1.0 -> 1.2.3
  v0.Increment(SemVer::kMinor);  // 1.2.3 -> 1.3.0 (patch is reset to 0)
  v1.Increment(SemVer::kMajor);  // 1.2.3 -> 2.0.0 (minor & patch are reset to 0)

  if (v1 > v0) {
    // 2.0.0 is greater than 1.3.0
    std::cout << v1.str() << " is greater than " << v0.str() << "\n";
  }

  return 0;
}

```

## License

*Semaver* is licensed under the [MIT License](https://opensource.org/licenses/MIT).