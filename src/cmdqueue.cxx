/** zsconfuz-common cmdqueue.cxx
    (C) 2019 Erik Zscheile
    License: ISC
 **/
#include "dflout.h"
#include "cmdqueue.hpp"
#include <string.h>          // strlen

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <iterator>
#include <iomanip>           // quoted
#include <unordered_map>
#include <vector>

using namespace std;

namespace zs {
namespace confuz {

static void merge_cmdqueue(cmdqueue_t &ret, cmdqueue_t &&tm) {
  if(!tm.output.empty()) ret.output = move(tm.output);
  ret.cmds.insert(ret.cmds.end(), move_iterator(tm.cmds.begin()), move_iterator(tm.cmds.end()));
}

[[gnu::cold]]
static void readcq_errmsg(const char * file, const size_t lnum, const string msg1, const string &msg2 = {}) {
  const size_t filelen = strlen(file);
  string em;
  em.reserve(23 + filelen + msg1.size() + msg2.size());
  (em += "cmdqueue file ").append(file, filelen);
  (em += ": line ") += to_string(lnum);
  (em += ": ") += msg1;
  em += msg2;
  throw cmdqueue_parse_error(move(em));
}

static const unordered_map<string, function<bool (cmdqueue_t &, vector<string> &)>> read_cq_jt = {
  { "output", [](cmdqueue_t &cdat, vector<string> &args) {
    if(args.size() != 1)
      return false;
    cdat.output = move(args.front());
    return true;
  }},
  { "include", [](cmdqueue_t &cdat, vector<string> &args) {
    if(args.size() != 1)
      return false;
    merge_cmdqueue(cdat, cmdqueue_t::read_from_file(args.front().c_str()));
    return true;
  }},
};

/* read_cmdqueue
 * reads the commands + output file name from the given cmdqueue file
 * NOTE: probably needs more optimization, we spend round 10% runtime here
 */
[[gnu::hot]]
auto cmdqueue_t::read_from_file(const char *file) -> cmdqueue_t {
  ifstream cqin(file);
  if(!cqin) throw cmdqueue_parse_error("can't open cmdqueue file: " + string(file));
  cmdqueue_t ret;
  ret.output = ZSCONFUZ_DFL_OUTPUT;

  string tmp;
  size_t lnum = -1;
  while(getline(cqin, tmp)) {
    ++lnum;
    istringstream ss(move(tmp));
    ss >> quoted(tmp);
    if(!ss) continue;

    if(tmp.size() == 1) {
      bool skipl = false;
      switch(tmp.front()) {
        case ':': {
            string section;
            ss >> quoted(section);
            if(!ss) readcq_errmsg(file, lnum, "invalid section (':') statement");
            ret.new_section(move(section));
          }
          [[fallthrough]];

        case '#':
          skipl = true;
        default : break;
      }
      if(skipl) continue;
    }

    string fi = move(tmp);
    vector<string> tmpv;
    while((ss >> quoted(tmp)))
      tmpv.emplace_back(move(tmp));

    if(ret.cmds.empty()) {
      const auto it = read_cq_jt.find(fi);
      if(it == read_cq_jt.end())
        readcq_errmsg(file, lnum, fi, ": unknown command");
      if(!it->second(ret, tmpv))
        readcq_errmsg(file, lnum, fi, ": got invalid arguments");
    } else {
      string &ccmd = ret.cmds.back().second.emplace_back();
      ccmd.reserve(1 + fi.size() + 3 * tmpv.size());
      ccmd += move(fi);
      for(auto &&i : tmpv) (ccmd += '\0') += move(i);
    }
  }

  return ret;
}

void cmdqueue_t::remove_empty_sections() {
  const auto ie = cmds.end();
  cmds.erase(std::remove_if(cmds.begin(), ie,
    [](const decltype(cmds)::value_type &i) noexcept -> bool { return i.second.empty(); }
  ), ie);
}

auto cmdqueue_t::serialize() const -> string {
  ostringstream ss;
  if(output != ZSCONFUZ_DFL_OUTPUT)
    ss << "output \"" << output << "\"\n\n";

  for(const auto &i : cmds) {
    ss << ": \"" << i.first << "\"\n";
    for(const auto &j : i.second) {
      ss << "\"";
      for(const auto c : j) {
        if(!c) ss << "\" \"";
        else   ss << c;
      }
      ss << "\"\n";
    }
    ss << '\n';
  }

  return ss.str();
}

bool cmdqueue_t::write_to_file(const char *file) const noexcept {
  try {
    ofstream cqout(file);
    if(cqout) cqout << serialize();
    return static_cast<bool>(cqout);
  } catch(...) {
    return false;
  }
}

}
}
