/** zsconfuz-common cqmerge.cxx
    (C) 2019 Erik Zscheile
    License: ISC

 This program reads an command queue dataset from stdin and writes it to stdout,
 but removes empty sections. It is used to validate the input and self-test our
 library.
 **/

#include "cmdqueue.hpp"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::string;

[[noreturn]]
static void errmsg(const int rc, const string &msg) {
  std::cerr << "zsconfuz-cqmerge: ERROR: " << msg << '\n';
  exit(rc);
}

int main(int argc, char *argv[]) {
  if(argc == 1) {
    std::cerr <<
      "USAGE: zsconfuz-cqmerge [-o CQ_OUTPUT] [-q] CQ_INPUTs...\n"
      "\n"
      "This program merges multiple command queues into one and\n"
      "outputs the merged command queue on stdout (use '-q' to suppress that)\n"
      "and optionally to the file given as argument to '-o'\n"
      "\n"
      "Return codes:\n"
      "  0 successful\n"
      "  1 usage error\n"
      "  2 cq read error\n"
      "  3 cq write error\n"
      "\n"
      "zsconfuz-common -- (C) 2019 Erik Kai Alain Zscheile\n -- ISC License\n";
    return 1;
  }

  const auto next_arg = [&argc, &argv]() {
    if(argc <= 0) return false;
    --argc;
    ++argv;
    return (argc > 0);
  };

  std::vector<string> outputs;
  bool is_quiet = false;

  // parse options
  while(next_arg() && argv[0][0] == '-' && argv[0][1] && !argv[0][2]) {
    switch(argv[0][1]) {
      case 'q':
        is_quiet = true;
        break;
      case 'o':
        if(!next_arg()) errmsg(1, "missing optarg for '-o'");
        outputs.emplace_back(argv[0]);
        break;
      default:
        errmsg(1, "got unknown option: " + string(argv[0]));
        break;
    }
  }

  zs::confuz::cmdqueue_t cq;

  // handle inputs
  try {
    for(; argc > 0; next_arg())
      cq.append_and_merge(zs::confuz::cmdqueue_t::read_from_file(argv[0], false));
  } catch(const zs::confuz::cmdqueue_parse_error &e) {
    std::cerr << "zsconfuz-cqsanitize: ERROR: " << e.what() << '\n';
    return 1;
  }

  cq.remove_empty_sections();
  const string serdat = cq.serialize();
  if(!is_quiet) std::cout << serdat;
  bool success = true;
  for(const auto &i : outputs) {
    std::ofstream cqout(i.c_str());
    if(!cqout) success = false;
    else cqout << serdat;
  }
  return success ? 0 : 3;
}
