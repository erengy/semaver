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
#include <string>
#include <vector>

#include "string.h"

namespace semaver {

bool IsNumericChar(const char c) {
  return c >= '0' && c <= '9';
}

bool IsNumericString(const std::string& str) {
  return !str.empty() &&
         std::all_of(str.begin(), str.end(), IsNumericChar);
}

////////////////////////////////////////////////////////////////////////////////

int CompareStrings(const std::string& str1, const std::string& str2) {
  return str1.compare(str2);
}

////////////////////////////////////////////////////////////////////////////////

std::string NumberToString(unsigned long number) {
  return std::to_string(number);
}

unsigned long StringToNumber(const std::string& str) {
  return std::stoul(str);
}

////////////////////////////////////////////////////////////////////////////////

std::vector<std::string> SplitString(const std::string& str,
                                     const std::string& delimiter) {
  std::vector<std::string> output;

  size_t offset = 0;
  while (true) {
    const size_t pos = str.find(delimiter, offset);
    if (pos == std::string::npos) {
      output.push_back(str.substr(offset));
      break;
    }
    output.push_back(str.substr(offset, pos - offset));
    offset = pos + delimiter.size();
  }

  return output;
}

}  // namespace semaver
