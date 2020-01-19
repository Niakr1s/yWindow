#include "loader.h"

#include "dictionary.h"
#include "exceptions.h"
#include "parser.h"

dict::Loader *dict::Loader::getFilesystemLoader(const fs::path &path,
                                                bool only_info) {
  if (!fs::exists(path)) {
    throw FSPathException("Loader::getFilesystemLoader: path doesn't exist",
                          path);
  }
  if (fs::is_directory(path)) {
    if (fs::is_empty(path)) {
      throw FSPathException("Loader::getFilesystemLoader: empty directory",
                            path);
    }
    return new DirectoryLoader(path, only_info);
  } else if (fs::is_regular_file(path)) {
    if (fs::is_empty(path)) {
      throw FSPathException("Loader::getFilesystemLoader: empty file", path);
    }
    return new FileLoader(path, only_info);
  }
  return nullptr;
}

dict::FilesystemLoader::FilesystemLoader(const fs::path &path, bool only_info)
    : path_(path), only_info_(only_info) {}

void dict::DirectoryLoader::doLoadInto(Dictionary *dict) {
  auto iter = fs::directory_iterator(path_);
  for (auto &p : iter) {
    auto parser = Parser::getParser(dict, p, only_info_);
    parser->parseInto(dict);
    delete parser;
  }
}

void dict::FileLoader::doLoadInto(dict::Dictionary *dict) {
  auto parser = Parser::getParser(dict, path_, only_info_);
  parser->parseInto(dict);
  delete parser;
}
