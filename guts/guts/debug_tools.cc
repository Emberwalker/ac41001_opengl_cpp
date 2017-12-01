#include "debug_tools.h"

namespace {

std::vector<GLenum> GetAllErrors() {
  std::vector<GLenum> errs;
  GLenum err;
  while ((err = gl::GetError()) != gl::NO_ERROR_) {
    errs.push_back(err);
  }
  return errs;
}

std::string OpenGLErrorToString(GLenum err) {
  switch (err) {
    case 0x500:
      return "GL_INVALID_ENUM";
    case 0x501:
      return "GL_INVALID_VALUE";
    case 0x502:
      return "GL_INVALID_OPERATION";
    case 0x503:
      return "GL_STACK_OVERFLOW";
    case 0x504:
      return "GL_STACK_UNDERFLOW";
    case 0x505:
      return "GL_OUT_OF_MEMORY";
    case 0x506:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case 0x507:
      return "GL_CONTEXT_LOST";
    default:
      return "Unknown error code";
  }
}

void PrintSingleError(GLenum err) {
  std::cerr
      << "OpenGL error 0x"
      << std::hex << err
      << ": " << OpenGLErrorToString(err)
      << std::endl;
}

} // namespace

namespace guts {

OGLStackReporter::OGLStackReporter(const std::string &caller,
                                   const std::string &file,
                                   int line) {
  this->caller_ = caller;
  this->file_ = file;
  this->line_ = line;
}

bool OGLStackReporter::operator()() {
  std::vector<GLenum> errs = GetAllErrors();
  if (errs.empty()) return false;
  for (auto &err : errs) {
    std::cerr << "[" << file_ << ":" << std::dec << line_ << "] "
              << caller_ << "(): ";
    PrintSingleError(err);
  }
  return true;
}

void __GutsAssert(const std::string &fn, const std::string &file, int ln,
                  bool condition, const std::string &msg) {
  if (condition) return;
  __GutsError(fn, file, ln, msg);
}

void __GutsError(const std::string &fn,
                 const std::string &file,
                 int ln,
                 const std::string &msg) {
  std::cerr << "[" << file << ":"
            << ln << "] "
            << "Assertion failure in " << fn
            << "(): " << msg << std::endl;
  exit(EXIT_FAILURE);
}

} // namespace guts

