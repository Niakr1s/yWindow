#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

class FSPathException : public std::exception {
 public:
  FSPathException(const std::string& msg, const fs::path& path);

  const char* what() const override;

 private:
  std::string msg_;
  fs::path path_;
};

#endif  // EXCEPTIONS_H
