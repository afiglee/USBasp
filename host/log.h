#ifndef _LOG_H__
#define _LOG_H__
#include <iostream>
#include <ostream>


enum LOG_LEVEL {
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

void set_log_level(enum LOG_LEVEL level);
enum LOG_LEVEL get_log_level();

class _debug : public std::ostream {
public:
  _debug(std::ostream & os) : std::ostream(os.rdbuf()) {
  }
  template <typename T>
  friend _debug& operator<< (_debug &me, const T &value) {
    if (get_log_level() <= DEBUG){
      (std::ostream&) me << value;
    }
    return me;
  }
};


class _info : public std::ostream {
public:
  _info(std::ostream & os) : std::ostream(os.rdbuf()) {
  }
  template <typename T>
  friend _info& operator<< (_info &me, const T &value) {
    if (get_log_level() <= INFO){
      (std::ostream&) me << value;
    }
    return me;
  }
};


class _warning : public std::ostream {
public:
  _warning(std::ostream & os) : std::ostream(os.rdbuf()) {
  }
  template <typename T>
  friend _warning& operator<< (_warning &me, const T &value) {
    if (get_log_level() <= WARNING){
      ((std::ostream&) me) << value;
    }
    return me;
  }
};

class _error : public std::ostream {
public:
  _error(std::ostream & os) : std::ostream(os.rdbuf()) {
  }
  template <typename T>
  friend _error& operator<< (_error &me, const T &value) {
    if (get_log_level() <= ERROR){
      ((std::ostream&) me) << value;
    }
    return me;
  }
};

extern _debug debug;
extern _info info;
extern _warning warning;
extern _error error;

#endif