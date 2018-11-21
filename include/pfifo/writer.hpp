#ifndef __PFIFO_WRITER_HPP__
#define __PFIFO_WRITER_HPP__

#include <string>
#include <system_error>

#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace pfifo {

class writer {
 public:
  writer(std::string name, mode_t mode) : name(name) {
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

  writer(std::string name) : name(name) {
    if ((fd = open(name.c_str(), O_RDWR)) == -1) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }
  }

  ~writer() {
    close(fd);
    unlink(name.c_str());
  }

  void write(std::string msg) const {
    std::size_t len = sizeof(char) * msg.length();
    char* buf = new char[sizeof(std::size_t) + len];

    *reinterpret_cast<std::size_t*>(buf) = len;
    std::copy(msg.begin(), msg.end(), buf + sizeof(std::size_t));

    std::size_t n = ::write(fd, buf, sizeof(std::size_t) + len);
    delete[] buf;

    if (n < 0) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }

    if (n != sizeof(std::size_t) + len) {
      throw std::system_error(EMSGSIZE, std::system_category());
    }
  }

 private:
  std::string name;
  int fd;
};

}  // namespace pfifo

#endif  // __PFIFO_WRITER_HPP__
