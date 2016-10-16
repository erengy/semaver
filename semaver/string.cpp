/*
** Taiga
** Copyright (C) 2010-2014, Eren Okka
** 
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <algorithm>
#include <string>
#include <vector>

#include "string.h"

using std::vector;
using std::wstring;

int CompareStrings(const wstring& str1, const wstring& str2,
                   bool case_insensitive, size_t max_count) {
  if (case_insensitive) {
    return _wcsnicmp(str1.c_str(), str2.c_str(), max_count);
  } else {
    return wcsncmp(str1.c_str(), str2.c_str(), max_count);
  }
}

bool IsNumericChar(const wchar_t c) {
  return c >= L'0' && c <= L'9';
}
bool IsNumericString(const wstring& str) {
  return !str.empty() &&
         std::all_of(str.begin(), str.end(), IsNumericChar);
}

void Split(const wstring& str, const wstring& separator,
           vector<wstring>& split_vector) {
  if (separator.empty()) {
    split_vector.push_back(str);
    return;
  }

  size_t index_begin = 0, index_end;

  do {
    index_end = str.find(separator, index_begin);
    if (index_end == wstring::npos)
      index_end = str.length();

    split_vector.push_back(str.substr(index_begin, index_end - index_begin));

    index_begin = index_end + separator.length();
  } while (index_begin <= str.length());
}

int ToInt(const wstring& str) {
  return _wtoi(str.c_str());
}

wstring ToWstr(const unsigned int& value) {
  wchar_t buffer[65];
  _ultow_s(value, buffer, 65, 10);
  return wstring(buffer);
}