# Semaver

A single-header C++ library for using [Semantic Versioning](https://semver.org) in your applications.

## Usage

```cpp
#include <iostream>
#include <semaver.hpp>

using Version = semaver::Version;

int main() {
  auto v = Version();    // 0.1.0
  v = Version(1, 2, 3);  // 0.1.0 → 1.2.3
  v.increment_major();   // 1.2.3 → 2.0.0

  if (v > Version("2.0.0-beta")) {  // 2.0.0 > 2.0.0-beta
    std::cout << v.to_string() << " > 2.0.0-beta\n";
  }

  if (auto v = Version("x.y.z"); !v) {
    std::cout << "Invalid version\n";
  }

  return 0;
}
```

## License

Licensed under the [MIT License](https://opensource.org/licenses/MIT).
