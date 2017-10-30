/*
Lab3start
Creates a cube and a sphere
Includes rotation, scaling, translation, view and perspective transformations,
and is setup for you to add lighting because the objects have normals and colours defined
for all vertices.
Feel free to either use this example as a start to lab3 or extract bits and add to
an example of your own to practice implementing per-vertex lighting.
Iain Martin October 2017
*/

/* Include the header to the GLFW wrapper class which
also includes the OpenGL extension initialisation*/
#include <guts/guts.h>
#include <iostream>

// We'll use the STD stack class to make our stack or matrices
#include <stack>

/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glload/gl_4_1.hpp>

// Include headers for our objects
//#include "sphere.h"
#include <guts/objs/cube.h>
#include <guts/objs/sphere.h>
#include <memory>

GLuint program;        /* Identifier for the shader program */
GLuint vao;            /* Vertex array (Container) object. This is the index of the VAO that will be the container for
					our buffer objects */

GLuint colourmode;    /* Index of a uniform to switch the colour mode in the vertex shader
					I've included this to show you how to pass in an unsigned integer into
					your vertex shader. */

/* Position and view globals */
GLfloat angle_x, angle_inc_x, x, model_scale, z, y;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
guts::GLRenderMode drawmode;            // Defines drawing mode of sphere as points, lines or filled polygons
GLuint numlats, numlongs;    //Define the resolution of the sphere object

/* Uniforms*/
GLint modelID, viewID, projectionID, nMatrixID;
GLint colourmodeID;

GLfloat aspect_ratio;        /* Aspect ratio of the window defined in the reshape callback*/
GLuint numspherevertices;

/* Global instances of our objects */
std::unique_ptr<guts::objs::Sphere> aSphere;
std::unique_ptr<guts::objs::Cube> aCube;

using namespace std;
using namespace glm;

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(guts::GlfwWindow *glw) {
  /* Set the object transformation controls to their initial values */
  x = 0;
  y = 0;
  z = 0;
  angle_x = angle_y = angle_z = 0;
  angle_inc_x = angle_inc_y = angle_inc_z = 0;
  model_scale = 1.f;
  aspect_ratio = 1024.f / 768.f;    // Initial aspect ratio from window size (variables would be better!)
  colourmode = 1;        // 0=use colour attrib buffer, 1=colour defined in shaders
  numlats = 20;        // Number of latitudes in our sphere
  numlongs = 20;        // Number of longitudes in our sphere

  // Generate index (name) for one vertex array object
  gl::GenVertexArrays(1, &vao);

  // Create the vertex array object and make it current
  gl::BindVertexArray(vao);

  /* Load and build the vertex and fragment shaders */
  try {
    guts::GLProgramBuilder builder;
    builder.AddShader(gl::VERTEX_SHADER, "lab3.vert");
    builder.AddShader(gl::FRAGMENT_SHADER, "lab3.frag");
    program = builder.BuildProgram();
  } catch (exception &e) {
    cout << "Caught exception: " << e.what() << endl;
    cin.ignore();
    exit(EXIT_FAILURE);
  }

  /* Define uniforms to send to vertex shader */
  modelID = gl::GetUniformLocation(program, "model");
  colourmodeID = gl::GetUniformLocation(program, "colourmode");
  viewID = gl::GetUniformLocation(program, "view");
  projectionID = gl::GetUniformLocation(program, "projection");
  nMatrixID = gl::GetUniformLocation(program, "n_matrix");

  /* create our sphere and cube objects */
  aCube = std::make_unique<guts::objs::Cube>();
  aSphere = std::make_unique<guts::objs::Sphere>(numlats, numlongs);
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

  // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  mat4 projection = perspective(30.0f, aspect_ratio, 0.1f, 100.0f);
//	mat4 projection = perspective(30.0f, 4.f/3.f, 0.1f, 100.0f);	// This strectches objecs with window resize

  // Camera matrix
  mat4 view = lookAt(
      vec3(0, 0, 4), // Camera is at (0,0,4), in World Space
      vec3(0, 0, 0), // and looks at the origin
      vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
  );

  // Send our projection and view uniforms to the currently bound shader
  // I do that here because they are the same for all objects
  gl::Uniform1ui(colourmodeID, colourmode);
  gl::UniformMatrix4fv(viewID, 1, gl::FALSE_, &view[0][0]);
  gl::UniformMatrix4fv(projectionID, 1, gl::FALSE_, &projection[0][0]);

  // Define our model transformation in a stack and
  // push the identity matrix onto the stack
  stack<mat4> model;
  model.push(mat4(1.0f));

  // Define our transformations that apply to all our objects
  // by modifying the matrix at the top of the stack
  model.top() = scale(model.top(), vec3(model_scale, model_scale, model_scale));//scale equally in all axis
  model.top() = rotate(model.top(), -angle_x, vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
  model.top() = rotate(model.top(), -angle_y, vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
  model.top() = rotate(model.top(), -angle_z, vec3(0, 0, 1)); //rotating in clockwise direction around z-axis

  // Push the current value of the top of the matrix onto the stack
  // I like to add a code block here to emphasise  the section between push and pop
  // This block of code draws the cube
  model.push(model.top());
  {
    // Define the model transformations for the cube
    model.top() = translate(model.top(), vec3(x + 0.5f, y, z));

    // Send the model uniform to the currently bound shader,
    gl::UniformMatrix4fv(modelID, 1, gl::FALSE_, &(model.top()[0][0]));

    // Send normal matrix for lighting
    mat3 n_matrix = guts::InverseTranspose(glm::mat3(view * model.top()));
    gl::UniformMatrix3fv(nMatrixID, 1, gl::FALSE_, &n_matrix[0][0]);

    /* Draw our cube*/
    aCube->Render(drawmode);
  }
  model.pop();

  // This block of code draws the sphere
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(-x - 0.5f, 0, 0));
    model.top() =
        scale(model.top(), vec3(model_scale / 3.f, model_scale / 3.f, model_scale / 3.f));//scale equally in all axis

    gl::UniformMatrix4fv(modelID, 1, gl::FALSE_, &(model.top()[0][0]));
    mat3 n_matrix = guts::InverseTranspose(glm::mat3(view * model.top()));
    gl::UniformMatrix3fv(nMatrixID, 1, gl::FALSE_, &n_matrix[0][0]);

    /* Draw our sphere */
    aSphere->Render(drawmode);
  }
  model.pop();

  gl::DisableVertexAttribArray(0);
  gl::UseProgram(0);

  /* Modify our animation variables */
  angle_x += angle_inc_x;
  angle_y += angle_inc_y;
  angle_z += angle_inc_z;
}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow *window, int w, int h) {
  gl::Viewport(0, 0, (GLsizei) w, (GLsizei) h);
  aspect_ratio = float(w) / float(h);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow *window, int key, int s, int action, int mods) {
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

  if (key == 'M' && action != GLFW_PRESS) colourmode = !colourmode;

  /* Cycle between drawing vertices, mesh and filled polygons */
  if (key == ',' && action != GLFW_PRESS) {
    drawmode = static_cast<guts::GLRenderMode>(drawmode + 1);
    if (drawmode > 2) drawmode = static_cast<guts::GLRenderMode>(0);
  }

}

/* Entry point of program */
int main(int argc, char *argv[]) {
  guts::GlfwWindow *glw = new guts::GlfwWindow(1024, 768, "Lab3 start example");

  // Register the callback functions
  glw->SetRenderer(display);
  glw->SetKeyCallback(keyCallback);
  glw->SetReshapeCallback(reshape);

  init(glw);

  glw->EventLoop();

  delete (glw);
  return 0;
}
