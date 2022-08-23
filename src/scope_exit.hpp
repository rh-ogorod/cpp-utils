// Hey Emacs, this is -*- coding: utf-8; mode: c++ -*-
#ifndef __rh_ogorod_cpp_utils_scope_exit_hpp__
#define __rh_ogorod_cpp_utils_scope_exit_hpp__

#include <vector>

namespace rh_ogorod::cpp_utils {

template <typename F>
requires std::invocable<F>
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct ScopeExit {
  using Finally = F;

  ScopeExit(const Finally&) = delete;
  auto operator=(const ScopeExit&) -> ScopeExit& = delete;

  explicit ScopeExit(Finally&& finally) : m_finally{std::move(finally)} {}

  ~ScopeExit() {
    m_finally();
  }

 private:
  Finally m_finally;
};

}  // namespace rh_ogorod::cpp_utils

#endif  // __rh_ogorod_cpp_utils_scope_exit_hpp__
