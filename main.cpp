#include <chrono>
#include <iostream>
#include <memory>
#include "fileswap_fstream.hpp"

// Helper functions
template <class Ratio, class F>
std::chrono::duration<double, Ratio> benchmark(F& func, unsigned times) {
  double average = 0;
  for (unsigned i = 0; i < times; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    average += std::chrono::duration<double, Ratio>(end - start).count();
  }

  return std::chrono::duration<double, Ratio>(average /
                                              static_cast<double>(times));
}

struct Test {
  int x;
  double d;
  char s[32];

  friend std::ostream& operator<<(std::ostream& os, const Test& t) {
    return os << '[' << t.x << ", " << t.d << ", " << t.s << "]\n";
  }
};

int main() {
  /*
    auto x = [] { std::cout << "dupa" << std::endl; };

    auto b = benchmark<std::micro>(x, 100);
    std::cout << b.count() << std::endl;
  */

  std::unique_ptr<FileSwap<FileSwapFstream>> swap(new FileSwapFstream);
  swap->open("swap");
  swap->put(10, 123);
  swap->put(200, Test{123, 3.14, "test!"});
  swap->put(24, 2137);

  auto x = swap->get<int>(10);
  std::cout << x.success << ", " << x.data << std::endl;

  x = swap->get<int>(24);
  std::cout << x.success << ", " << x.data << std::endl;

  // TODO: Recognition if there's a value under address, because right now this
  // will succeed. It shouldn't.
  x = swap->get<int>(123);
  std::cout << x.success << ", " << x.data << std::endl;

  auto s = swap->get<Test>(200);
  std::cout << s.success << ", " << s.data << std::endl;
}
