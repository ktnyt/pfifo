#ifndef __PFIFO_READER_HPP__
#define __PFIFO_READER_HPP__

#include <string>
#include <system_error>
#include <vector>

#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace pfifo {

class reader {
 public:
  reader(std::string name, mode_t mode) : name(name) {
    if (mkfifo(name.c_str(), mode) == -1) {
      int err = errno;
      if (err != EEXIST) {
        throw std::system_error(err, std::system_category());
      }
    }

    if ((fd = open(name.c_str(), O_RDWR)) == -1) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }
  }

  reader(std::string name) : name(name) {
    if ((fd = open(name.c_str(), O_RDWR)) == -1) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }
  }

  virtual ~reader() {
    close(fd);
    unlink(name.c_str());
  }

  std::vector<char> readv() {
    std::size_t n, len;
    n = ::read(fd, reinterpret_cast<char*>(&len), sizeof(std::size_t));

    if (n < 0) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }

    if (n != sizeof(std::size_t)) {
      throw std::system_error(EMSGSIZE, std::system_category());
    }

    if (len == 0) {
      return std::vector<char>();
    }

    std::vector<char> ret(len);
    n = ::read(fd, ret.data(), len);

    if (n < 0) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }

    if (n != len) {
      throw std::system_error(EMSGSIZE, std::system_category());
    }

    return ret;
  }

  std::string read() {
    std::vector<char> v = readv();
    return std::string(v.begin(), v.end());
  }

 private:
  std::string name;
  int fd;
};

}  // namespace pfifo

#endif  // __PFIFO_READER_HPP__
