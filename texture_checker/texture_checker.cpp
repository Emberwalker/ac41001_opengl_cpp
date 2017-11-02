/*
 Texture_checker
 Basic example to show how to texture a polygon
 Uses a checkerboard texture patter.
 Addapted from Example 6.1 in the Redbook V4.3
 Iain Martin October 2014
*/

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include <guts/guts.h>
#include <guts/objs/cube.h>
#include <iostream>

/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

/* Define buffer object indices */
GLuint quad_vbo, quad_normals, quad_colours, quad_tex_coords;

/* Define textureID*/
GLuint texID;

GLuint program;        /* Identifier for the shader prgoram */
GLuint vao;            /* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					   our buffer objects */

GLuint colourmode;    /* Index of a uniform to switch the colour mode in the vertex shader
					  I've included this to show you how to pass in an unsigned integer into
					  your vertex shader. */

/* Position and view globals */
GLfloat angle_x, angle_inc_x, x, model_scale, z, y;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
GLuint
    drawmode;            // Defines drawing mode of sphere as points, lines or filled polygons

/* Uniforms*/
GLint modelID, viewID, projectionID;
GLint colourmodeID;

GLfloat
    aspect_ratio;        /* Aspect ratio of the window defined in the reshape callback*/

std::unique_ptr<guts::objs::Cube> cube;

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(guts::GlfwWindow *glw) {
  /* Set the object transformation controls to their initial values */
  x = 0;
  y = 0;
  z = 0;
  angle_y = angle_z = 0;
  angle_x = -20.f;
  angle_inc_x = angle_inc_y = angle_inc_z = 0;
  model_scale = 1.f;
  aspect_ratio = 1.3333f;
  colourmode = 1;

  // Generate index (name) for one vertex array object
  gl::GenVertexArrays(1, &vao);

  // Create the vertex array object and make it current
  gl::BindVertexArray(vao);

  /* Load and build the vertex and fragment shaders */
  try {
    auto builder = new guts::GLProgramBuilder();
    builder->AddShader(gl::VERTEX_SHADER, "texture_checker.vert");
    builder->AddShader(gl::FRAGMENT_SHADER, "texture_checker.frag");
    program = builder->BuildProgram();
  } catch (exception &e) {
    cout << "Caught exception: " << e.what() << endl;
    cin.ignore();
    exit(0);
  }

  /* Define uniforms to send to vertex shader */
  modelID = gl::GetUniformLocation(program, "model");
  colourmodeID = gl::GetUniformLocation(program, "colourmode");
  viewID = gl::GetUniformLocation(program, "view");
  projectionID = gl::GetUniformLocation(program, "projection");

  guts::PrintOpenGLErrors();

  /* Create our quad and texture */
  /*gl::GenBuffers(1, &quad_vbo);
  gl::BindBuffer(gl::ARRAY_BUFFER, quad_vbo);*/

  // Create dat for our quad with vertices, normals and texturee coordinates
  /*static const GLfloat quad_data[] =
      {
          // Vertex positions
          0.75f, 0, -0.75f,
          -0.75f, 0, -0.75f,
          -0.75f, 0, 0.75f,
          0.75f, 0, 0.75f,

          // Normals
          0, 1.f, 0,
          0, 1.f, 0,
          0, 1.f, 0,
          0, 1.f, 0,

          // Texture coordinates. Note we only need two per vertex but have a
          // redundant third to fit the texture coords in the same buffer for this simple object
          0.0f, 0.0f, 0,
          5.0f, 0.0f, 0,
          5.0f, 5.0f, 0,
          0.0f, 5.0f, 0,
      };

  // Copy the data into the buffer. See how this example combines the vertices, normals and texture
  // coordinates in the same buffer and uses the last parameter of  glVertexAttribPointer() to
  // specify the byte offset into the buffer for each vertex attribute set.
  gl::BufferData(gl::ARRAY_BUFFER,
                 sizeof(quad_data),
                 quad_data,
                 gl::STATIC_DRAW);

  gl::VertexAttribPointer(0, 3, gl::FLOAT, gl::FALSE_, 0, (void *) (0));
  gl::VertexAttribPointer(1,
                          3,
                          gl::FLOAT,
                          gl::FALSE_,
                          0,
                          (void *) (12 * sizeof(float)));
  gl::VertexAttribPointer(2,
                          3,
                          gl::FLOAT,
                          gl::FALSE_,
                          0,
                          (void *) (24 * sizeof(float)));

  gl::EnableVertexAttribArray(0);
  gl::EnableVertexAttribArray(1);
  gl::EnableVertexAttribArray(2);*/

  // Generate the texture object, obtaining a texture ID for the 2D texture object
  gl::GenTextures(1, &texID);
  gl::BindTexture(gl::TEXTURE_2D, texID);
  gl::TexStorage2D(gl::TEXTURE_2D, 4, gl::RGBA8, 8, 8);

  /* Define a texture, normally we would load an image file instead but for
     some simple textures it is reasonable to just define the bytes in an array */
  static const unsigned char texture_data[] =
      {
          0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
          0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
          0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
          0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
          0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
          0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
          0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
          0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF
      };

  /* Specify the texture object. */
  /* void glTexSubImage2D(GLenum target, 	GLint level, GLint xoffset, GLint yoffset,
                          GLsizei width,	GLsizei height,	GLenum format,
                          GLenum type, const GLvoid * data);
  You could also use glTexImage2D. Look up both of these functions
  gl::RED is used to specify one colour component, we use this to make white using the swizzle below */
  gl::TexSubImage2D(gl::TEXTURE_2D, 0, 0, 0,
                    8, 8, gl::RED,
                    gl::UNSIGNED_BYTE, texture_data);

  guts::PrintOpenGLErrors();

  /* Convert the single red texture component into RGB, i.e. grey scale which will
    be white or black in our checker texture */
  static const GLint swizzles[] = {gl::RED, gl::RED, gl::RED, gl::ONE};
  gl::TexParameteriv(gl::TEXTURE_2D, gl::TEXTURE_SWIZZLE_RGBA, swizzles);

  guts::PrintOpenGLErrors();

  /* Define the texture behaviour parameters */
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::REPEAT);
  gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::REPEAT);

  guts::PrintOpenGLErrors();

  // This is the location of the texture object (TEXTURE0), i.e. tex1 will be the name
  // of the sampler in the fragment shader
  int loc = gl::GetUniformLocation(program, "tex1");
  if (loc >= 0) gl::Uniform1i(loc, 0);

  guts::PrintOpenGLErrors();

  cube = std::make_unique<guts::objs::Cube>(true, 0, 2, 1);
}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
   class because we registered display as a callback function */
void display(guts::GlfwWindow *window) {
  /* Define the background colour */
  gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  /* Clear the colour and frame buffers */
  gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

  /* Enable depth test  */
  gl::Enable(gl::DEPTH_TEST);

  /* Make the compiled shader program current */
  gl::UseProgram(program);

  // Define the model transformations
  mat4 model = mat4(1.0f);
  model = translate(model, vec3(x, y, z));
  model = scale(model,
                vec3(model_scale * 5,
                     model_scale * 5,
                     model_scale * 5));//scale equally in all axis
  model = rotate(model,
                 -angle_x,
                 vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
  model = rotate(model,
                 -angle_y,
                 vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
  model = rotate(model,
                 -angle_z,
                 vec3(0, 0, 1)); //rotating in clockwise direction around z-axis

  // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  mat4 Projection = perspective(30.0f, aspect_ratio, 0.1f, 100.0f);

  // Camera matrix
  mat4 View = lookAt(
      vec3(0, 0, 4), // Camera is at (0,0,4), in World Space
      vec3(0, 0, 0), // and looks at the origin
      vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
  );

  // Send our uniforms variables to the currently bound shader,
  gl::UniformMatrix4fv(modelID, 1, gl::FALSE_, &model[0][0]);
  gl::Uniform1ui(colourmodeID, colourmode);
  gl::UniformMatrix4fv(viewID, 1, gl::FALSE_, &View[0][0]);
  gl::UniformMatrix4fv(projectionID, 1, gl::FALSE_, &Projection[0][0]);

  /* Draw our textured quad*/
  gl::BindTexture(gl::TEXTURE_2D, texID);
  //gl::DrawArrays(gl::TRIANGLE_FAN, 0, 4);
  cube->Render(static_cast<guts::GLRenderMode>(drawmode));

  /* Modify our animation variables */
  angle_x += angle_inc_x;
  angle_y += angle_inc_y;
  angle_z += angle_inc_z;
}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow *window, int w, int h) {
  gl::Viewport(0, 0, (GLsizei) w, (GLsizei) h);
  aspect_ratio = ((float) w / 640.f * 4.f) / ((float) h / 480.f * 3.f);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow *window,
                        int key,
                        int s,
                        int action,
                        int mods) {
  /* Enable this call if you want to disable key responses to a held down key*/
  //if (action != GLFW_PRESS) return;

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, gl::TRUE_);

  if (key == 'Q') angle_inc_x -= 0.05f;
  if (key == 'W') angle_inc_x += 0.05f;
  if (key == 'E') angle_inc_y -= 0.05f;
  if (key == 'R') angle_inc_y += 0.05f;
  if (key == 'T') angle_inc_z -= 0.05f;
  if (key == 'Y') angle_inc_z += 0.05f;
  if (key == 'A') model_scale -= 0.02f;
  if (key == 'S') model_scale += 0.02f;
  if (key == 'Z') x -= 0.05f;
  if (key == 'X') x += 0.05f;
  if (key == 'C') y -= 0.05f;
  if (key == 'V') y += 0.05f;
  if (key == 'B') z -= 0.05f;
  if (key == 'N') z += 0.05f;

  if (key == 'M' && action != GLFW_PRESS) {
    colourmode = !colourmode;
    cout << "colourmode=" << colourmode << endl;
  }

  /* Cycle between drawing vertices, mesh and filled polygons */
  if (key == 'N' && action != GLFW_PRESS) {
    drawmode++;
    if (drawmode > 2) drawmode = 0;
  }

}

/* Entry point of program */
int main(int argc, char *argv[]) {
  auto *glw = new guts::GlfwWindow(1024, 768, "Texture checker example");;

  glw->SetRenderer(display);
  glw->SetKeyCallback(keyCallback);
  glw->SetReshapeCallback(reshape);

  init(glw);

  glw->EventLoop();

  delete (glw);
  return 0;
}

