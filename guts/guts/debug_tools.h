#ifndef GUTS_DEBUG_TOOLS_H
#define GUTS_DEBUG_TOOLS_H

#include <string>
#include <iostream>
#include <vector>
#include <glload/gl_4_1.hpp>

namespace guts {

// Based on https://stackoverflow.com/a/378165
class OGLStackReporter {
 public:
  OGLStackReporter(const std::string &caller, const std::string &file,
                   int line);

  bool operator()();

 private:
  std::string   caller_;
  std::string   file_;
  int           line_;

};

#undef PrintOpenGLErrors
#define PrintOpenGLErrors OGLStackReporter(__FUNCTION__,__FILE__,__LINE__)

void __GutsAssert(const std::string &fn, const std::string &file, int ln,
                  bool condition, const std::string &msg);

} // namespace guts

// Prepend a parameter with __unused to silence 'unused parameter' warnings.
#undef __unused
#define __unused __attribute__((unused))

#undef guts_assert
#define guts_assert(condition, msg) guts::__GutsAssert(__FUNCTION__,__FILE__,__LINE__, condition, msg)

#undef NOT_IMPLEMENTED
#define NOT_IMPLEMENTED { guts::__GutsAssert(__FUNCTION__,__FILE__,__LINE__, false, "Not implemented."); }

#endif //GUTS_DEBUG_TOOLS_H
