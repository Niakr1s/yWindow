#ifndef LOADER_H
#define LOADER_H

#include <filesystem>
#include <future>

#include "card.h"

namespace fs = std::filesystem;

namespace dict {

class Dictionary;

class Loader {
 private:
  template <class Dict>
  static std::future<Dictionary*> loadFromFS(const fs::path& dir,
                                             bool with_info) {
    return std::async([=] {
      Dictionary* dict = new Dict();
      auto loader = getFilesystemLoader(dir, with_info);
      loader->doLoadInto(dict);
      delete loader;
      return dict;
    });
  }

 public:
  virtual ~Loader() = default;

  template <class Dict>
  static std::future<Dictionary*> loadFromFS(const fs::path& dir) {
    return loadFromFS<Dict>(dir, false);
  }

  template <class Dict>
  static std::future<Dictionary*> loadFromFSInfo(const fs::path& dir) {
    return loadFromFS<Dict>(dir, true);
  }

 protected:
  virtual void doLoadInto(Dictionary* dict) = 0;

  static Loader* getFilesystemLoader(const fs::path& path,
                                     bool only_info = false);
};

class FilesystemLoader : public Loader {
  friend class Loader;

 protected:
  FilesystemLoader(const fs::path& path, bool only_info);

  fs::path path_;
  bool only_info_;
};

class DirectoryLoader : public FilesystemLoader {
  friend class Loader;

 protected:
  using FilesystemLoader::FilesystemLoader;

  void doLoadInto(Dictionary* dict) override;
};

class FileLoader : public FilesystemLoader {
  friend class Loader;

 protected:
  using FilesystemLoader::FilesystemLoader;

  void doLoadInto(Dictionary* dict) override;
};

// todo ZipLoader etc

}  // namespace dict

#endif  // LOADER_H
