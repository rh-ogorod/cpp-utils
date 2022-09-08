// Hey Emacs, this is -*- coding: utf-8 -*-
#ifndef __rh_ogorod_cpp_utils_binary_form_hpp__
#define __rh_ogorod_cpp_utils_binary_form_hpp__

#include <bitset>
#include <climits>
#include <iostream>

template <typename T>
class BinaryForm {
 public:
  explicit BinaryForm(const T& v) : _bs(v) {}

 private:
  const std::bitset<sizeof(T) * CHAR_BIT> _bs;

  template <typename U>
  friend auto operator<<(std::ostream& os, const BinaryForm<U>& bf)
      -> std::ostream&;
};

template <typename T>
inline auto operator<<(std::ostream& os, const BinaryForm<T>& bf)
    -> std::ostream& {
  return os << bf._bs;
}

#endif // __rh_ogorod_cpp_utils_binary_form_hpp__
