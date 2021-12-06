#ifndef COMMON_CORE_FILE_H
#define COMMON_CORE_FILE_H

#include <string>
#include <fstream>
#include <memory>

class File {
  public:
    enum class Result {
      NoError,
      EmptyFilePath,
      CannotOpen,
      AlreadyOpen
    };

    enum class Type {
      R, // read
      W, // write
      RW // read/write
    };

  public:
    File(const std::string& path, const Type type);
    File(const File& file) = delete;
    virtual ~File();

    Result open();
    void close();
  public:
    File& operator=(const File&) = delete;

  private:
    std::string mFilePath;
    Type mType;

    std::unique_ptr<std::fstream> mStream;
};

#endif // COMMON_CORE_FILE_H