/** zsconfuz-common write.cxx
    (C) 2019 Erik Zscheile
    License: ISC
 **/

#include "write.hpp"
#include <string.h>

namespace zs {
namespace confuz {

[[gnu::hot]]
intern::sv intern::obj2sv(const char *x) noexcept
  { return { x, strlen(x) }; }

}
}
