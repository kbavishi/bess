#ifndef BESS_UTILS_ENDIAN_H_
#define BESS_UTILS_ENDIAN_H_

#include <cstdint>

namespace bess {
namespace utils {

constexpr bool is_be_system() {
  return (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__);
}

template <typename T>
class EndianBase {
 public:
  static constexpr T swap(const T &v);
};

template <>
class EndianBase<uint16_t> {
 public:
  static constexpr uint16_t swap(const uint16_t &v) {
    return __builtin_bswap16(v);
  }
};

template <>
class EndianBase<uint32_t> {
 public:
  static constexpr uint32_t swap(const uint32_t &v) {
    return __builtin_bswap32(v);
  }
};

template <>
class EndianBase<uint64_t> {
 public:
  static constexpr uint64_t swap(const uint64_t &v) {
    return __builtin_bswap64(v);
  }
};

template <typename T>
class[[gnu::packed]] BigEndian final : public EndianBase<T> {
 public:
  BigEndian() = default;
  BigEndian(const BigEndian<T> &o) = default;

  constexpr BigEndian(const T &cpu_value)
      : data_(is_be_system() ? cpu_value : EndianBase<T>::swap(cpu_value)) {}

  BigEndian<T> &operator=(const T &cpu_value) {
    data_ = is_be_system() ? cpu_value : EndianBase<T>::swap(cpu_value);
    return *this;
  }

  constexpr T value() {
    return is_be_system() ? data_ : EndianBase<T>::swap(data_);
  }

  constexpr T raw_value() { return data_; }

  constexpr BigEndian<T> operator&(const BigEndian<T> &o) {
    return BigEndian(data_ & o.data_);
  }

  constexpr BigEndian<T> operator|(const BigEndian<T> &o) {
    return BigEndian(data_ | o.data_);
  }

  constexpr BigEndian<T> operator~() { return BigEndian(~data_); }

  constexpr bool operator==(const BigEndian &o) { return data_ == o.data_; }

  constexpr bool operator!=(const BigEndian &o) { return !(*this == o); }

  constexpr friend bool operator==(const BigEndian<T> &l, const T &r) {
    return __builtin_constant_p(r) ? (l == BigEndian<T>(r)) : l.value() == r;
  }

  constexpr friend bool operator==(const T &l, const BigEndian<T> &r) {
    return r == l;
  }

  constexpr friend bool operator!=(const BigEndian<T> &l, const T &r) {
    return !(l == r);
  }

  constexpr friend bool operator!=(const T &l, const BigEndian<T> &r) {
    return !(l == r);
  }

 protected:
  T data_;  // stored in big endian in memory
};

using be16_t = BigEndian<uint16_t>;
using be32_t = BigEndian<uint32_t>;
using be64_t = BigEndian<uint64_t>;

// POD means trivial (no special constructor/destructor) and
// standard layout (i.e., binary compatible with C struct)
static_assert(std::is_pod<be16_t>::value, "not a POD type");
static_assert(std::is_pod<be32_t>::value, "not a POD type");
static_assert(std::is_pod<be64_t>::value, "not a POD type");

static_assert(sizeof(be16_t) == 2, "be16_t is not 2 bytes");
static_assert(sizeof(be32_t) == 4, "be32_t is not 4 bytes");
static_assert(sizeof(be64_t) == 8, "be64_t is not 8 bytes");

// this is to make sure BigEndian has constexpr constructor and value()
static_assert(be32_t(0x1234).value() == 0x1234, "Something is wrong");

}  // namespace utils
}  // namespace bess

#endif  // BESS_UTILS_ENDIAN_H_
