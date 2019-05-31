/** zsconfuz-common cmdqueue.hpp
    (C) 2019 Erik Zscheile
    License: ISC
 **/
#pragma once
#include <deque>
#include <istream>
#include <stdexcept>
#include <string>
#include <utility>

namespace zs {
namespace confuz {
  struct cmdqueue_parse_error : public std::logic_error {
    using std::logic_error::logic_error;
  };

  struct cmdqueue_t {
    typedef std::deque<std::string> insec_cmds_queue_t;

    std::string output;
    std::deque<std::pair<std::string, insec_cmds_queue_t>> cmds;
    void new_section(std::string &&name) {
      cmds.emplace_back(std::make_pair(
        std::move(name),
        insec_cmds_queue_t()));
    }

    static auto read_from_stream(std::istream &cqin, const char *file) -> cmdqueue_t;
    static auto read_from_file(const char *file) -> cmdqueue_t;
    static auto cmd2argv(const std::string &cmd) -> std::deque<std::string>;
    static auto arg2quoted(const std::string &arg) -> std::string;

    void remove_empty_sections();
    void append_and_merge(cmdqueue_t &&tm);
    bool valid() const noexcept { return !(output.empty() && cmds.empty()); }
    auto serialize() const -> std::string;
    bool write_to_file(const char *file) const noexcept;
  };
}
}
