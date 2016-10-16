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

#ifndef TAIGA_BASE_STRING_H
#define TAIGA_BASE_STRING_H

#include <string>
#include <vector>

int CompareStrings(const std::wstring& str1, const std::wstring& str2, bool case_insensitive, size_t max_count);

bool IsNumericChar(const wchar_t c);
bool IsNumericString(const std::wstring& str);

void Split(const std::wstring& str, const std::wstring& separator, std::vector<std::wstring>& split_vector);

int ToInt(const std::wstring& str);

std::wstring ToWstr(const unsigned int& value);

#endif  // TAIGA_BASE_STRING_H
