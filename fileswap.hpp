#ifndef FILESWAP_HPP
#define FILESWAP_HPP

// Rust-like
template <typename T>
struct Result {
  T data;
  bool success;
};

template <class Impl>
class FileSwap {
 public:
  template <typename T>
  Result<T> get(std::size_t address) {
    return static_cast<Impl*>(this)->template get<T>(address);
  }

  template <typename T>
  void put(std::size_t address, const T& data) {
    static_cast<Impl*>(this)->template put(address, data);
  }

  bool open(const char* filename) {
    return static_cast<Impl*>(this)->open(filename);
  }

  void close() { return static_cast<Impl*>(this)->close(); }
};

#endif
