#ifndef __PFIFO_READER_HPP__
#define __PFIFO_READER_HPP__

#include <string>
#include <system_error>

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
  std::string read() {
    std::size_t n, len;
    n = ::read(fd, reinterpret_cast<char*>(&len), sizeof(std::size_t));

    if (n < 0) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }

    if (n != sizeof(std::size_t)) {
      throw std::system_error(EMSGSIZE, std::system_category());
    }

    char* buf = new char[len];
    n = ::read(fd, buf, len);

    if (n < 0) {
      int err = errno;
      delete[] buf;
      throw std::system_error(err, std::system_category());
    }

    if (n != len) {
      delete[] buf;
      throw std::system_error(EMSGSIZE, std::system_category());
    }

    std::string ret(buf);
    delete[] buf;

    return ret;
  }

 private:
  std::string name;
  int fd;
};

}  // namespace pfifo

#endif  // __PFIFO_READER_HPP__
