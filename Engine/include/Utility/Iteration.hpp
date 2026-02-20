#ifndef EHAZ_ITERATION_HPP
#define EHAZ_ITERATION_HPP

#include <type_traits>

namespace eHaz {

template <typename E> static constexpr auto to_underlying(E e) noexcept {
  return static_cast<std::underlying_type_t<E>>(e);
}

template <typename E> class EnumIterator {
  int value;

public:
  explicit EnumIterator(int v) : value(v) {}
  E operator*() const { return static_cast<E>(value); }
  EnumIterator &operator++() {
    ++value;
    return *this;
  }
  bool operator!=(const EnumIterator &other) const {
    return value != other.value;
  }
};

template <typename E> class EnumRange {
  int begin_value, end_value;

public:
  EnumRange(E begin, E end)
      : begin_value(to_underlying(begin)), end_value(to_underlying(end)) {}
  EnumIterator<E> begin() const { return EnumIterator<E>(begin_value); }
  EnumIterator<E> end() const { return EnumIterator<E>(end_value + 1); }
};

template <typename E> EnumRange<E> static enum_range(E begin, E end) {
  return EnumRange<E>(begin, end);
}

} // namespace eHaz

#endif
