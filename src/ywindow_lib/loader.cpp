#include "loader.h"

#include "dictionary.h"
#include "parser.h"

void dict::Loader::loadInto(Dictionary *dict) { return doLoadInto(dict); }

dict::Loader *dict::Loader::getFilesystemLoader(const fs::path &path) {
  if (!fs::exists(path)) {
    throw std::invalid_argument(
        "Loader::getFilesystemLoader: path doesn't exist");
  }
  if (fs::is_directory(path)) {
    return new DirectoryLoader(path);
  }  // todo else return ziploader etc
  return nullptr;
}

dict::FilesystemLoader::FilesystemLoader(const fs::path &path) : path_(path) {}

dict::DirectoryLoader::DirectoryLoader(const fs::path &path)
    : FilesystemLoader(path) {}

void dict::DirectoryLoader::doLoadInto(Dictionary *dict) {
  auto iter = fs::directory_iterator(path_);
  for (auto &p : iter) {
    try {
      auto parser = Parser::getParser(p);
      parser->parseInto(dict);
    } catch (std::exception &e) {
      std::cout << e.what();
    }
  }
}
