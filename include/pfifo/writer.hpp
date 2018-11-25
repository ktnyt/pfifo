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

    if ((fd = open(name.c_str(), O_WRONLY)) == -1) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }
  }

  writer(std::string name) : name(name) {
    if ((fd = open(name.c_str(), O_WRONLY)) == -1) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }
  }

  ~writer() {
    close(fd);
    unlink(name.c_str());
  }

  inline void write_size(std::size_t size) const {
    char* buf = reinterpret_cast<char*>(&size);
    std::size_t n = ::write(fd, buf, sizeof(std::size_t));

    if (n < 0) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }

    if (n != sizeof(std::size_t)) {
      throw std::system_error(EMSGSIZE, std::system_category());
    }
  }

  template <class Sequence>
  void write(Sequence msg) const {
    flock(fd, LOCK_EX);

    std::size_t size = sizeof(typename Sequence::value_type) * msg.size();
    const char* buf = reinterpret_cast<const char*>(msg.data());

    write_size(size);

    std::size_t written = 0;

    while (written < size) {
      written += ::write(fd, buf + written, size - written);
    }

    if (written != size) {
      throw std::system_error(EMSGSIZE, std::system_category());
    }

    flock(fd, LOCK_UN);
  }

 private:
  std::string name;
  int fd;
};

template <>
void writer::write(const char* msg) const {
  write(std::string(msg));
}

}  // namespace pfifo

#endif  // __PFIFO_WRITER_HPP__
