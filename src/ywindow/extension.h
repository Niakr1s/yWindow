#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <string>

struct InfoForExtension {
  const char* name;
  int64_t value;
};

struct SentenceInfo {
  const InfoForExtension* infoArray;
  int64_t operator[](std::string propertyName) {
    for (auto info = infoArray; info->name;
         ++info)  // nullptr name marks end of info array
      if (propertyName == info->name) return info->value;
    return *(int*)0xcccc = 0;  // gives better error message than alternatives
  }
};

struct SKIP {};
inline void Skip() { throw SKIP(); }

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                    LPVOID lpReserved);

bool ProcessSentence(std::wstring& sentence, SentenceInfo sentenceInfo);

extern "C" __declspec(dllexport) wchar_t* OnNewSentence(
    wchar_t* sentence, const InfoForExtension* sentenceInfo);
