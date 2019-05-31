/** zsconfuz-common write.hpp
    (C) 2019 Erik Zscheile
    License: ISC
 **/

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/uio.h>
#include <functional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace zs {
namespace confuz {

  namespace intern {
    using sv = std::string_view;
    using std::vector;

    template<class T>
    static inline sv obj2sv(const T &obj, const size_t offset = 0) noexcept
      { return { obj.data() + offset, obj.size() - offset }; }

    static inline sv obj2sv(const char &x) noexcept
      { return { &x, 1 }; }

    sv obj2sv(const char *x) noexcept;

    static inline iovec sv2iov(const sv &x) noexcept
      { return { const_cast<void*>(static_cast<const void*>(x.data())), x.size() }; }

    template<class Tout>
    static inline void accumulate2buf(vector<Tout> &buf, const std::function<Tout (const sv &)> &trf) noexcept
      { }

    template<class Tout, class Tin, class... Types>
    static inline void accumulate2buf(vector<Tout> &buf, const std::function<Tout (const sv &)> &trf, const Tin &val, const Types&... args) {
      buf.emplace_back(trf(obj2sv(val)));
      accumulate2buf(buf, trf, args...);
    }

    template<class Tout, class... Types>
    static inline auto obj2xv2(const std::function<Tout (const sv &)> &trf, const Types&... args) {
      vector<Tout> ret;
      ret.reserve(1 + sizeof...(Types));
      accumulate2buf(ret, trf, args...);
      return ret;
    }

    template<class... Types>
    static inline auto objs2iov2(const Types&... args) {
      return obj2xv2<iovec, Types...>(sv2iov, args...);
    }

    template<class... Types>
    static inline auto objs2sv2(const Types&... args) {
      return obj2xv2<sv, Types...>([](const sv &x) { return x; }, args...);
    }
  }

  template<class... Types>
  static inline ssize_t write_obj(const int fd, const Types&... args) noexcept {
    const auto iovx = intern::obj2sv(args...);
    return write(fd, iovx.data(), iovx.size());
  }

  template<class... Types>
  static inline ssize_t write_objs(const int fd, const Types&... args) {
    ssize_t ret;
    const auto iov2 = intern::objs2iov2(args...);
    do { ret = writev(fd, iov2.data(), iov2.size()); }
      while(ret == -1 && errno == EINTR);
    return ret;
  }

  template<class... Types>
  static inline void buffer_obj(std::string &buf, const Types&... args) {
    const auto x = intern::obj2sv(args...);
    buf.append(x.data(), x.size());
  }

  template<class T>
  static inline void buffer_objvm(
      std::string &buf,
      const std::function<size_t (const T &)> &grsf,
      const std::function<void (std::string &, const T &)> &trf,
      const std::vector<T>& args
  ) {
    size_t n = buf.size();
    for(const auto &i : args)
      n += grsf(i);
    buf.reserve(n);
    for(const auto &i : args)
      trf(buf, i);
  }

  // this template allows T to be auto-deduced
  //  (without this template, the compiler would complain about incompatible function types)
  template<class T, class FnS, class FnT>
  static inline void buffer_objvm(
      std::string &buf,
      FnS &&grsf,
      FnT &&trf,
      const std::vector<T>& args
  ) {
    return buffer_objvm<T>(buf,
      std::function<size_t (const T &)>(grsf),
      std::function<void (std::string &, const T &)>(trf),
      args);
  }

  template<class... Types>
  static inline void buffer_objs(std::string &buf, const Types&... args) {
    buffer_objvm<intern::sv>(buf,
      [](const intern::sv &x)                 { return x.size(); },
      [](std::string &b, const intern::sv &x) { b.append(x.data(), x.size()); },
      intern::objs2sv2(args...));
  }
}
}
