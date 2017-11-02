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

void PrintSingleError(GLenum err) {
  std::cerr
      << "OpenGL error 0x"
      << std::hex << err
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
    std::cerr << "[" << file_ << ":" << line_ << "] " << caller_ << "(): ";
    PrintSingleError(err);
  }
  return true;
}

void __GutsAssert(const std::string &fn, const std::string &file, int ln,
                  bool condition, const std::string &msg) {
  if (condition) return;
  std::cerr << "[" << file << ":"
            << ln << "] "
            << "Assertion failure in " << fn
            << "(): " << msg << std::endl;
  exit(EXIT_FAILURE);
}

} // namespace guts

