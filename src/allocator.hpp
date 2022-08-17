// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __rh_ogorod_cpp_utils_allocator_hpp__
#define __rh_ogorod_cpp_utils_allocator_hpp__

#include <concepts>
#include <memory>

namespace rh_ogorod::cpp_utils {

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
requires std::invocable<Construct, T*> && std::invocable<Destroy, T*> &&
  std::movable<Construct> && std::movable<Destroy>
auto allocateShared(Construct&& construct, Destroy&& destroy) noexcept {
  return std::allocate_shared<T>(
    // NOLINTNEXTLINE(bugprone-move-forwarding-reference)
    Allocator<T, Construct, Destroy>(std::move(construct), std::move(destroy))
  );
}

}  // namespace rh_ogorod::cpp_utils

#endif  // __rh_ogorod_cpp_utils_allocator_hpp__
