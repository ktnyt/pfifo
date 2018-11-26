#ifndef __PFIFO_READER_HPP__
#define __PFIFO_READER_HPP__

#include <string>
#include <system_error>
#include <vector>

#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/file.h>
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

    if ((fd = open(name.c_str(), O_RDONLY)) == -1) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }
  }

  reader(std::string name) : name(name) {
    if ((fd = open(name.c_str(), O_RDONLY)) == -1) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }
  }

  virtual ~reader() {
    close(fd);
    unlink(name.c_str());
  }

  inline std::size_t read_size() {
    std::size_t size;
    char* buf = reinterpret_cast<char*>(&size);
    std::size_t n = ::read(fd, buf, sizeof(std::size_t));

    if (n < 0) {
      int err = errno;
      throw std::system_error(err, std::system_category());
    }

    if (n != sizeof(std::size_t)) {
      throw std::system_error(EMSGSIZE, std::system_category());
    }

    return size;
  }

  template <class Sequence>
  Sequence read() {
    std::size_t size = read_size();
    if (size == 0) {
      return Sequence();
    }

    Sequence ret(size / sizeof(typename Sequence::value_type));
    char* buf = reinterpret_cast<char*>(ret.data());

    std::size_t readout = 0;
    while (readout < size) {
      readout += ::read(fd, buf + readout, size - readout);
    }

    return ret;
  }

  std::string reads() {
    auto v = read<std::vector<char>>();
    return std::string(v.begin(), v.end());
  }

 private:
  std::string name;
  int fd;
};

}  // namespace pfifo

#endif  // __PFIFO_READER_HPP__
