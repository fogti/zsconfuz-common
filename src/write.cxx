/** zsconfuz-common write.cxx
    (C) 2019 Erik Zscheile
    License: ISC
 **/

#include "write.hpp"
#include <string.h>

namespace zs {
namespace confuz {

[[gnu::hot]]
intern::iovec intern::obj2iovec(const char *x) noexcept
  { return { const_cast<void*>(static_cast<const void*>(x)), strlen(x) }; }

}
}
