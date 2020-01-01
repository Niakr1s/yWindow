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

  void loadInto(Dictionary* dict);
  std::future<void> loadIntoAsync(Dictionary* dict);

  static Loader* getFilesystemLoader(const fs::path& path);

 protected:
  virtual void doLoadInto(Dictionary* dict) = 0;
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

 private:
  string getDictionaryInfo();
};

// todo ZipLoader etc

}  // namespace dict

#endif  // LOADER_H
