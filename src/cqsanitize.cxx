/** zsconfuz-common cqsanitize.cxx
    (C) 2019 Erik Zscheile
    License: ISC

 This program reads an command queue dataset from stdin and writes it to stdout,
 but removes empty sections. It is used to validate the input and self-test our
 library.
 **/

#include "cmdqueue.hpp"
#include <iostream>

int main() {
  try {
    auto cq = zs::confuz::cmdqueue_t::read_from_stream(std::cin, "<stdin>");
    cq.remove_empty_sections();
    std::cout << cq.serialize();
    return 0;
  } catch(const zs::confuz::cmdqueue_parse_error &e) {
    std::cerr << "zsconfuz-cqsanitize: ERROR: " << e.what() << '\n';
  }
  return 1;
}
