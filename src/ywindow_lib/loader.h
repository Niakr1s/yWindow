#ifndef LOADER_H
#define LOADER_H

#include <filesystem>
#include <future>

#include "card.h"

namespace fs = std::filesystem;

namespace dict {

class Dictionary;

class Loader {
 public:
  virtual ~Loader() = default;

  template <class Dict>
  static std::future<Dictionary*> loadFromFS(const fs::path& dir) {
    return std::async([dir] {
      Dictionary* dict = new Dict();
      auto loader = getFilesystemLoader(dir);
      loader->doLoadInto(dict);
      return dict;
    });
  }

 protected:
  virtual void doLoadInto(Dictionary* dict) = 0;

  static Loader* getFilesystemLoader(const fs::path& path);
};

class FilesystemLoader : public Loader {
  friend class Loader;

 protected:
  FilesystemLoader(const fs::path& path);

  fs::path path_;
};

class DirectoryLoader : public FilesystemLoader {
  friend class Loader;

 protected:
  DirectoryLoader(const fs::path& path);

  void doLoadInto(Dictionary* dict) override;
};

class FileLoader : public FilesystemLoader {
  friend class Loader;

 protected:
  FileLoader(const fs::path& path);

  void doLoadInto(Dictionary* dict) override;
};

// todo ZipLoader etc

}  // namespace dict

#endif  // LOADER_H
