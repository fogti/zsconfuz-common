/** zsconfuz-common write.hpp
    (C) 2019 Erik Zscheile
    License: ISC
 **/

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string>
#include <utility>
#include <vector>

namespace zs {
namespace confuz {

  namespace intern {
    using iovec = struct iovec;

    template<class T>
    static inline iovec obj2iovec(const T &obj, const size_t offset = 0) noexcept {
      return { const_cast<void*>(static_cast<const void*>(obj.data() + offset)),
               obj.size() - offset };
    }

    static inline iovec obj2iovec(const char &x) noexcept
      { return { const_cast<void*>(static_cast<const void*>(&x)), 1 }; }

    iovec obj2iovec(const char *x) noexcept;

    static inline void accumulate_iovec(std::vector<iovec> &buf) noexcept { }

    template<class T, class... Types>
    static inline void accumulate_iovec(std::vector<iovec> &buf, const T &val, const Types&... args) {
      buf.emplace_back(obj2iovec(val));
      accumulate_iovec(buf, args...);
    }

    template<class... Types>
    static inline auto objs2iov2(const Types&... args) {
      std::vector<intern::iovec> ret;
      ret.reserve(1 + sizeof...(Types));
      accumulate_iovec(ret, args...);
      return ret;
    }
  }

  template<class... Types>
  static inline ssize_t write_obj(const int fd, const Types&... args) noexcept {
    const auto iovx = intern::obj2iovec(args...);
    return writev(fd, &iovx, 1);
  }

  template<class... Types>
  static inline ssize_t write_objs(const int fd, const Types&... args) noexcept {
    ssize_t ret;
    const auto iov2 = intern::objs2iov2(args...);
    do { ret = writev(fd, iov2.data(), iov2.size()); }
      while(ret == -1 && errno == EINTR);
    return ret;
  }

  template<class... Types>
  static inline void buffer_obj(std::string &buf, const Types&... args) noexcept {
    const auto iovx = intern::obj2iovec(args...);
    buf.append(static_cast<const char*>(iovx.iov_base), iovx.iov_len);
  }

  template<class... Types>
  static inline void buffer_objs(std::string &buf, const Types&... args) noexcept {
    const auto iov2 = intern::objs2iov2(args...);
    {
      size_t n = buf.size();
      for(const auto &i : iov2)
        n += i.iov_len;
      buf.reserve(n);
    }
    for(const auto &i : iov2)
      buf.append(static_cast<const char*>(i.iov_base), i.iov_len);
  }
}
}
