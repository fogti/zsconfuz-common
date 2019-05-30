/** zsconfuz-common filter.c
    (C) 2019 Erik Zscheile
    License: ISC

 * USAGE: CMD ARGS... | zsconfuz-filter
 * filters all zsconfuz control codes to prevent malicious code execution
 **/

#include <unistd.h>
#include <stdbool.h>

static void zs_write_range(const char * begin, const char * end) {
  if(end <= begin) return;
  write(1, begin, end - begin);
}

int main() {
  bool got_nl = true;
  char buf[1024] = {0};
  while(1) {
    const int rdr = read(0, buf, sizeof(buf) - 1);
    if(rdr < 1) break;
    const char *mark = buf, *eobuf = buf + rdr;
    for(const char * pos = buf; pos < eobuf; ++pos) {
      if(*pos == '\n') {
        got_nl = true;
        continue;
      }
      bool filtch = false;
      if(!got_nl) {
        switch(*pos) {
          case '\004': // worker terminated
          case '\006': // worker finished job
            filtch = true;
        }
      } else {
        got_nl = false;
        switch(*pos) {
          case '\000': // redirect output to shell results file
          case '\001': // runtime section push
          case '\002': // runtime command push
            filtch = true;
        }
      }
      if(filtch) {
        zs_write_range(mark, pos);
        mark = pos + 1;
      }
    }
    zs_write_range(mark, eobuf);
  }
  return 0;
}
