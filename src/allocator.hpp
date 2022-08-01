// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __rh_ogorod_cpp_utils_allocator_hpp__
#define __rh_ogorod_cpp_utils_allocator_hpp__

#include "functional.hpp"

namespace rh_ogorod::cpp_utils {

template <typename T>
class Allocator : public std::allocator<T> {
 public:
  using value_type = T;
  using pointer = value_type*;

  // using Construct = std::function<void(pointer ptr)>;
  // using Destroy = std::function<void(pointer ptr)>;
  using Construct = Function<void(pointer ptr)>;
  using Destroy = Function<void(pointer ptr) noexcept>;

  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  Allocator(Construct&& construct, Destroy&& destroy) noexcept
      : std::allocator<T>(),
        m_construct{std::move(construct)},
        m_destroy{std::move(destroy)} {};

  template <class U>
  explicit Allocator(const Allocator<U>& other) noexcept
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

}  // namespace rh_ogorod::cpp_utils

#endif  // __rh_ogorod_cpp_utils_allocator_hpp__
