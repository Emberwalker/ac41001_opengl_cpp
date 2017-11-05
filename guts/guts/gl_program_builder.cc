#include "gl_program_builder.h"

#include "guts/debug_tools.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace guts {

namespace {

std::string GetShaderSrc(const std::string &file_name) {
  std::ifstream file_stream;
  file_stream.open(file_name);

  if (!file_stream.is_open()) {
    std::cerr << "Could not read file " << file_name << "." << std::endl;
    throw std::invalid_argument("File " + file_name + " doesn't exist.");
  }

  std::stringstream strm;
  strm << file_stream.rdbuf();
  auto content = strm.str();

  file_stream.close();
  return content;
}

typedef void(*GlGetIvFn)(GLuint id, GLenum pname, GLint *params);
typedef void(*GlGetInfoLogFn)(GLuint id, GLsizei max_len, GLsizei *len,
                              GLchar *info_log);

std::string GetGLError(GlGetIvFn get_iv_fn, GlGetInfoLogFn info_fn, GLuint id) {
  GLint info_log_len;
  get_iv_fn(id, gl::INFO_LOG_LENGTH, &info_log_len);

  auto *info_log_cstr = new GLchar[info_log_len + 1];
  info_fn(id, info_log_len, nullptr, info_log_cstr);

  std::string log(info_log_cstr);
  delete[](info_log_cstr);
  return log;
}

} // namespace

GLProgramBuilder::~GLProgramBuilder() {
  for (GLuint &shader : this->shaders) {
    gl::DeleteShader(shader);
  }
}

void GLProgramBuilder::AddShader(GLenum shader_type,
                                 const std::string &shader_file_name) {
  guts_assert(!shader_file_name.empty(), "shader file name must not be empty");
  std::string shader_src;
  try {
    shader_src = GetShaderSrc(shader_file_name);
  } catch (std::invalid_argument &ex) {
    return; // Don't try and load an invalid shader.
  }

  GLuint shader = gl::CreateShader(shader_type);
  const char *shader_src_cstr = shader_src.c_str();
  gl::ShaderSource(shader, 1, &shader_src_cstr, nullptr);
  gl::CompileShader(shader);

  GLint status;
  gl::GetShaderiv(shader, gl::COMPILE_STATUS, &status);
  if (status == gl::FALSE_) {
    std::string info_log_str = GetGLError(gl::GetShaderiv, gl::GetShaderInfoLog,
                                          shader);
    std::string shader_type_str;
    switch (shader_type) {
      case gl::VERTEX_SHADER:shader_type_str = "vertex";
        break;
      case gl::GEOMETRY_SHADER:shader_type_str = "geometry";
        break;
      case gl::FRAGMENT_SHADER:shader_type_str = "fragment";
        break;
      default:shader_type_str = "unknown";
        break;
    }

    std::cerr << "Compile error in " << shader_type_str << " shader:\n\t"
              << info_log_str << std::endl;

    throw std::runtime_error("Shader compile exception");
  }

  this->shaders.push_back(shader);
}

GLuint GLProgramBuilder::BuildProgram() {
  if (this->already_built) {
    std::cerr << "[BUG] This builder has already generated a program. This "
              << "might represent a bug in your code." << std::endl;
  }

  GLuint program = gl::CreateProgram();
  for (GLuint &shader : this->shaders) {
    gl::AttachShader(program, shader);
  }
  gl::LinkProgram(program);

  GLint result;
  gl::GetProgramiv(program, gl::LINK_STATUS, &result);
  if (result == gl::FALSE_) {
    std::string info_log_str = GetGLError(gl::GetProgramiv,
                                          gl::GetProgramInfoLog, program);
    std::cerr << "Link error: \n\t" << info_log_str << std::endl;
    throw std::runtime_error("Link error");
  }

  this->already_built = true;
  return program;
}

} // namespace guts