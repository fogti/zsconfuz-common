/** zsconfuz-common push.cxx
    (C) 2019 Erik Zscheile
    License: ISC
 **/
#include "cmdqueue.hpp"
#include "dflout.h"
#include "write.hpp"

// STDC
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// STL
#include <functional>
#include <string>
#include <unordered_map>

using std::string;
using zs::confuz::write_objs;

static int cq_push_file(const char **file) {
  try {
    auto config = zs::confuz::cmdqueue_t::read_from_file(*file);

    config.remove_empty_sections();
    if(config.output != ZSCONFUZ_DFL_OUTPUT)
      write_objs(2, "zsconfuz-push: WARNING: ignore redirection to ", config.output, '\n');

    string fmt;
    for(const auto &section : config.cmds) {
      // section
      write_objs(1, '\001', section.first, '\n');
      for(const auto &cmd : section.second) {
        // command
        write_objs(1, '\002', cmd, '\n');
      }
    }
    return 0;
  } catch(const zs::confuz::cmdqueue_parse_error &e) {
    write_objs(2, "zsconfuz-push: ERROR: ", e.what(), '\n');
    return 2;
  }
}

static int cq_push_section(const char **section) {
  write_objs(1, '\001', *section, '\n');
  return 0;
}

static int cq_push_command(const char **argv) {
  write_objs(1, '\002');
  write_objs(1, *argv);
  for(++argv; *argv; ++argv) write_objs(1, '\0', *argv);
  write_objs(1, '\n');
  return 0;
}

static std::unordered_map<string, std::function<int (const char **argv)>> cqpfns = {
  { "file", cq_push_file },
  { "section", cq_push_section },
  { "command", cq_push_command },
};

int main(int argc, const char *argv[]) {
  if(argc < 3) {
    puts("USAGE: zsconfuz-push file CQ_FILE\n"
         "       zsconfuz-push section SECTION_NAME\n"
         "       zsconfuz-push command CMD ARGS...\n"
         "\nReturn codes:\n"
         "  0 successful\n"
         "  1 usage error\n"
         "  2 cq-file error\n"
         "zsconfuz-common -- (C) 2019 Erik Kai Alain Zscheile\n -- ISC License"
    );
    return 1;
  }

  const auto fnit = cqpfns.find(argv[1]);
  if(fnit == cqpfns.end()) {
    write_objs(2, "zsconfuz-push: ERROR: subcommand not found\n");
    return 1;
  }

  return (fnit->second)(argv + 2);
}
