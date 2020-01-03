#include "loader.h"

#include "dictionary.h"
#include "parser.h"

dict::Loader *dict::Loader::getFilesystemLoader(const fs::path &path) {
  if (!fs::exists(path)) {
    throw FSPathException("Loader::getFilesystemLoader: path doesn't exist",
                          path);
  }
  if (fs::is_directory(path)) {
    if (fs::is_empty(path)) {
      throw FSPathException("Loader::getFilesystemLoader: empty directory",
                            path);
    }
    return new DirectoryLoader(path);
  } else if (fs::is_regular_file(path)) {
    if (fs::is_empty(path)) {
      throw FSPathException("Loader::getFilesystemLoader: empty file", path);
    }
    return new FileLoader(path);
  }
  return nullptr;
}

dict::FilesystemLoader::FilesystemLoader(const fs::path &path) : path_(path) {}

dict::DirectoryLoader::DirectoryLoader(const fs::path &path)
    : FilesystemLoader(path) {}

dict::FileLoader::FileLoader(const fs::path &path) : FilesystemLoader(path) {}

void dict::DirectoryLoader::doLoadInto(Dictionary *dict) {
  auto iter = fs::directory_iterator(path_);
  for (auto &p : iter) {
    auto parser = Parser::getParser(dict, p);
    parser->parseInto(dict);
    delete parser;
  }
}

void dict::FileLoader::doLoadInto(dict::Dictionary *dict) {
  auto parser = Parser::getParser(dict, path_);
  parser->parseInto(dict);
  delete parser;
}
