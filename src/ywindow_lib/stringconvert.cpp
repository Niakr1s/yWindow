#include "stringconvert.h"

#include <Windows.h>

#include <boost/algorithm/string.hpp>
#include <vector>

std::string WideStringToString(const std::wstring &text) {
  std::vector<char> buffer((text.size() + 1) * 4);
  WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer.data(),
                      buffer.size(), nullptr, nullptr);
  return buffer.data();
}

std::wstring StringToWideString(const std::string &text) {
  std::vector<wchar_t> buffer(text.size() + 1);
  MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, buffer.data(),
                      buffer.size());
  return buffer.data();
}

void trimStr(std::string &str) { boost::trim(str); }

void trimStr(std::vector<std::string> &str_vec) {
  for (auto &str : str_vec) {
    trimStr(str);
  }
}
