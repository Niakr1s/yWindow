#pragma once

#include <string>
#include <vector>

std::wstring StringToWideString(const std::string& text);

std::string WideStringToString(const std::wstring& text);

void trimStr(std::string& str);

void trimStr(std::vector<std::string>& str_vec);
