#include "exceptions.h"

FSPathException::FSPathException(const std::string &msg, const fs::path &path)
    : std::exception(msg.c_str()), msg_(msg), path_(path) {}

const char *FSPathException::what() const {
  std::string res = msg_ + ": " + path_.string();
  return res.c_str();
}
