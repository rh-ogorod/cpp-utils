// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __rh_ogorod_cpp_utils_allocator_hpp__
#define __rh_ogorod_cpp_utils_allocator_hpp__

#include <memory>

namespace rh_ogorod::cpp_utils {

// Concepts:
// https://en.cppreference.com/w/cpp/concepts/invocable
// https://en.cppreference.com/w/cpp/concepts/movable
// https://en.cppreference.com/w/cpp/concepts/copyable

template <typename T, typename C, typename D>
class Allocator : public std::allocator<T> {
 public:
  using value_type = T;
  using pointer = value_type*;

  using Construct = C;
  using Destroy = D;

  Allocator(Construct&& construct, Destroy&& destroy) noexcept
      : std::allocator<T>(),
        m_construct{std::move(construct)},
        m_destroy{std::move(destroy)} {};

  template <typename UT, typename UC, typename UD>
  explicit Allocator(const Allocator<UT, UC, UD>& other) noexcept
      : std::allocator<T>(other) {}

  void construct(pointer ptr) {
    m_construct(ptr);
  }

  void destroy(pointer ptr) {
    m_destroy(ptr);
  }

 private:
  Construct m_construct;
  Destroy m_destroy;
};

template <typename T, typename Construct, typename Destroy>
auto allocateShared(Construct&& construct, Destroy&& destroy) {
  return std::allocate_shared<T>(
    Allocator<T, Construct, Destroy>(std::move(construct), std::move(destroy))
  );
}

}  // namespace rh_ogorod::cpp_utils

#endif  // __rh_ogorod_cpp_utils_allocator_hpp__
