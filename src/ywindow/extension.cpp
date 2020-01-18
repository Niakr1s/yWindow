#include "extension.h"

#include <QDebug>
#include <QString>

#include "settings.h"
#include "ywindow.h"

static YWindow* ywindow;

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                    LPVOID lpReserved) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      ywindow = new YWindow();
      QMetaObject::invokeMethod(
          ywindow, [] { ywindow->show(); }, Qt::QueuedConnection);
      break;
    case DLL_PROCESS_DETACH:
      delete ywindow;
      delete SETTINGS;
      break;
  }
  return TRUE;
}

/*
        Param sentence: sentence received by Textractor (UTF-16). Can be
   modified, Textractor will receive this modification only if true is returned.
        Param sentenceInfo: contains miscellaneous info about the sentence (see
   README). Return value: whether the sentence was modified. Textractor will
   display the sentence after all extensions have had a chance to process and/or
   modify it. The sentence will be destroyed if it is empty or if you call
   Skip(). This function may be run concurrently with itself: please make sure
   it's thread safe. It will not be run concurrently with DllMain.
*/
bool ProcessSentence(std::wstring& sentence, SentenceInfo sentenceInfo) {
  if (sentenceInfo["current select"] && sentenceInfo["text number"] != 0)
    QMetaObject::invokeMethod(ywindow,
                              [sentence = QString::fromStdWString(sentence)] {
                                ywindow->newText(sentence);
                              });
  return false;
}

extern "C" __declspec(dllexport) wchar_t* OnNewSentence(
    wchar_t* sentence, const InfoForExtension* sentenceInfo) {
  try {
    std::wstring sentenceStr(sentence);
    int origLength = sentenceStr.size();
    if (ProcessSentence(sentenceStr, SentenceInfo{sentenceInfo})) {
      if (sentenceStr.size() > origLength)
        sentence = (wchar_t*)HeapReAlloc(
            GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS, sentence,
            (sentenceStr.size() + 1) * sizeof(wchar_t));
      wcscpy_s(sentence, sentenceStr.size() + 1, sentenceStr.c_str());
    }
  } catch (SKIP) {
    *sentence = L'\0';
  }
  return sentence;
}
