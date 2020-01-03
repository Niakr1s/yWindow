#pragma once

#include <string>

std::wstring StringToWideString(const std::string& text);

std::string WideStringToString(const std::wstring& text);
