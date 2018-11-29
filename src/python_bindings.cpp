#include "pfifo/pfifo.hpp"

#include <pybind11/pybind11.h>

#include <memory>
#include <string>
#include <vector>

namespace py = pybind11;

class reader final {
 public:
  reader(std::string name, py::object mode) {
    if (mode.is(py::none())) {
      ptr = new pfifo::reader(name);
    } else {
      ptr = new pfifo::reader(name, py::cast<mode_t>(mode));
    }
  }

  ~reader() { delete ptr; }

  py::bytes read() { return py::bytes(ptr->read<char*>()); }

 private:
  pfifo::reader* ptr;
};

class writer final {
 public:
  writer(std::string name, py::object mode) {
    if (mode.is(py::none())) {
      ptr = new pfifo::writer(name);
    } else {
      ptr = new pfifo::writer(name, py::cast<mode_t>(mode));
    }
  }

  ~writer() { delete ptr; }

  void write(std::string msg) { ptr->write(msg); }

 private:
  pfifo::writer* ptr;
};

PYBIND11_MODULE(_pfifo, m) {
  py::class_<reader>(m, "Reader")
      .def(py::init<std::string, py::object>())
      .def("read", &reader::read);

  py::class_<writer>(m, "Writer")
      .def(py::init<std::string, py::object>())
      .def("write", &writer::write);
}
