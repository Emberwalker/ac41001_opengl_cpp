#ifndef GUTS_DEBUG_TOOLS_H
#define GUTS_DEBUG_TOOLS_H

#include <string>
#include <iostream>
#include <vector>
#include <glload/gl_4_1.hpp>

namespace guts {

// Used internally by PrintOpenGLErrors.
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

// Prints out all the current OpenGL errors in the context. Includes file/line
// and calling function in the error message. That allows you to just
// scatter-shot these statements everywhere there *might* be an issue.
#undef PrintOpenGLErrors
#define PrintOpenGLErrors OGLStackReporter(__FUNCTION__,__FILE__,__LINE__)

// Internal. Used by the guts_assert macro.
void __GutsAssert(const std::string &fn, const std::string &file, int ln,
                  bool condition, const std::string &msg);

// Internal. Used by the guts_assert macro.
[[noreturn]]
void __GutsError(const std::string &fn, const std::string &file, int ln,
                 const std::string &msg);

} // namespace guts

// Prepend a parameter with __unused to silence 'unused parameter' warnings.
#undef __unused
#ifdef _MSC_VER // MSVC
// Windows doesn't like this.
#define __unused
#else
#define __unused __attribute__((unused))
#endif // _MSC_VER

// Custom assertion method that includes a message, and tells you exactly where
// something failed an assertion.
#undef guts_assert
#define guts_assert(condition, msg) guts::__GutsAssert(__FUNCTION__,__FILE__,__LINE__, condition, msg)

// Error method for probably-never-happen type issues. Unlike guts_assert, this
// is marked as non-returning. Equivalent to calling guts_assert(false, "...");
#undef guts_error
#define guts_error(msg) guts::__GutsError(__FUNCTION__,__FILE__,__LINE__, msg)

// Handy little macro for marking things that aren't quite finished yet.
#undef NOT_IMPLEMENTED
#define NOT_IMPLEMENTED { guts::__GutsAssert(__FUNCTION__,__FILE__,__LINE__, false, "Not implemented."); }

#endif //GUTS_DEBUG_TOOLS_H
