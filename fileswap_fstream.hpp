#ifndef FILESWAP_FSTREAM_HPP
#define FILESWAP_FSTREAM_HPP
#include <cstring>
#include <fstream>
#include <iostream>
#include "fileswap.hpp"

class FileSwapFstream : public FileSwap<FileSwapFstream> {
 public:
  // First sizeof(std::size_t) bytes will be reserved for swap file size.
  FileSwapFstream() : _size(sizeof(std::size_t)) {}

  bool open(const char* filename) {
    _file.open(filename, std::ios::in | std::ios::out);
    if (_file.is_open()) {
      get_size();
      if (_size == 0) {
        set_size(sizeof(std::size_t));
      }
    } else {
      _file.open(filename, std::ios::out);
      _file.close();
      open(filename);
    }
    return true;
  }

  void close() { _file.close(); }

  template <typename T>
  Result<T> get(std::size_t address) {
    if (_size < (address + sizeof(T))) return Result<T>{T{}, false};

    // Seek to address
    _file.seekg(address);

    // Create data buffer
    char data[sizeof(T)];

    // Read to data buffer
    _file.read(data, sizeof(T));

    // Create result and copy the data
    Result<T> r;
    std::memcpy(&r.data, data, sizeof(T));
    r.success = true;

    return r;
  }

  template <typename T>
  void put(std::size_t address, const T& data) {
    if (_size < address) {
      _file.seekp(_size);
      // Reserve 32bit blocks of data until the size will be greater than
      // address
      char buf[32];
      std::memset(buf, 32, sizeof(buf));
      while (_size < address + sizeof(T)) {
        _file.write(buf, sizeof(buf));
        _size += sizeof(buf);
      }
      set_size(_size);
      _file.flush();
    }

    _file.seekp(address);
    char buf[sizeof(T)];
    std::memcpy(buf, &data, sizeof(T));
    _file.write(buf, sizeof(T));
  }

 private:
  void get_size() {
    _file.seekg(0);
    _file >> _size;

    if (_file.fail()) {
      _size = 0;
      _file.clear();
      _file.sync();
    }
  }

  void set_size(std::size_t size) {
    _file.seekp(0);
    _size = size;
    _file << size;
    _file.flush();
  }

  std::fstream _file;
  std::size_t _size;
};

#endif
