/** zsconfuz-common write.cxx
    (C) 2019 Erik Zscheile
    License: ISC
 **/

#include "write.hpp"
#include <string.h>

namespace zs {
namespace confuz {

ssize_t write_obj(const int fd, const char *x)
  { return write(fd, x, strlen(x)); }

}
}
