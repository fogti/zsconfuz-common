/** zsconfuz-common write.hpp
    (C) 2019 Erik Zscheile
    License: ISC
 **/

#include <errno.h>
#include <stddef.h>
#include <unistd.h>

namespace zs {
namespace confuz {
  template<class T>
  static inline ssize_t write_obj(const int fd, const T &obj, const size_t offset = 0)
    { return write(fd, obj.data() + offset, obj.size() - offset); }

  static inline ssize_t write_obj(const int fd, const char x)
    { return write(fd, &x, 1); }

  ssize_t write_obj(const int fd, const char *x);

  static inline ssize_t write_objs(const int fd)
    { return 0; }

  template<class T, class... Types>
  static inline ssize_t write_objs(const int fd, const T &val, const Types&... args) {
    ssize_t ret;
    do { ret = write_obj(fd, val); }
      while(ret == -1 && errno == EINTR);
    if(ret < 0) return ret;

    const ssize_t ret2 = write_objs(fd, args...);
    return (ret2 < 0) ? ret2 : (ret + ret2);
  }
}
}
