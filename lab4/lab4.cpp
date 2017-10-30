/*
 poslight.cpp
 Demonstrates a positional light with attenuation
 Displays a cube and a sphere and a small sphere to show the light position
 Includes controls to move the light source and rotate the view
 Iain Martin October 2017
*/

#include <guts/guts.h>
#include <iostream>
#include <stack>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <guts/objs/cube.h>
#include <guts/objs/sphere.h>
#include <memory>

/* Define buffer object indices */
GLuint elementbuffer;

GLuint program;        /* Identifier for the shader prgoram */
GLuint vao;            /* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					   our buffer objects */

GLuint colourmode;    /* Index of a uniform to switch the colour mode in the vertex shader
					  I've included this to show you how to pass in an unsigned integer into
					  your vertex shader. */
GLuint emitmode;

/* Position and view globals */
GLfloat angle_x, angle_inc_x, x, model_scale, z, y, vx, vy, vz;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
GLuint
    drawmode;            // Defines drawing mode of sphere as points, lines or filled polygons
GLuint numlats, numlongs;    //Define the resolution of the sphere object

GLfloat light_x, light_y, light_z;

/* Uniforms*/
GLint modelID, viewID, projectionID, lightposID, normalmatrixID;
GLint colourmodeID, emitmodeID;

GLfloat
    aspect_ratio;        /* Aspect ratio of the window defined in the reshape callback*/

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
  x = 0.05f;
  y = 0;
  z = 0;
  vx = 0;
  vx = 0, vz = 4.f;
  light_x = 0;
  light_y = 0;
  light_z = 0;
  angle_x = angle_y = angle_z = 0;
  angle_inc_x = angle_inc_y = angle_inc_z = 0;
  model_scale = 1.f;
  aspect_ratio = 1.3333f;
  colourmode = 0;
  emitmode = 0;
  numlats = 200;        // Number of latitudes in our sphere
  numlongs = 200;        // Number of longitudes in our sphere

  // Generate index (name) for one vertex array object
  gl::GenVertexArrays(1, &vao);

  // Create the vertex array object and make it current
  gl::BindVertexArray(vao);

  /* Load and build the vertex and fragment shaders */
  try {
    guts::GLProgramBuilder builder;
    builder.AddShader(gl::VERTEX_SHADER, "lab4.vert");
    builder.AddShader(gl::FRAGMENT_SHADER, "lab4.frag");
    program = builder.BuildProgram();
  } catch (exception &e) {
    cout << "Caught exception: " << e.what() << endl;
    cin.ignore();
    exit(0);
  }

  /* Define uniforms to send to vertex shader */
  modelID = gl::GetUniformLocation(program, "model");
  colourmodeID = gl::GetUniformLocation(program, "colourmode");
  emitmodeID = gl::GetUniformLocation(program, "emitmode");
  viewID = gl::GetUniformLocation(program, "view");
  projectionID = gl::GetUniformLocation(program, "projection");
  lightposID = gl::GetUniformLocation(program, "lightpos");
  normalmatrixID = gl::GetUniformLocation(program, "normalmatrix");

  /* create our sphere and cube objects */
  aSphere = std::make_unique<guts::objs::Sphere>(numlats, numlongs);
  aCube = std::make_unique<guts::objs::Cube>();
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

  // Define our model transformation in a stack and
  // push the identity matrix onto the stack
  stack<mat4> model;
  model.push(mat4(1.0f));

  // Define the normal matrix
  mat3 normalmatrix;

  // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  mat4 projection = perspective(30.0f, aspect_ratio, 0.1f, 100.0f);

  // Camera matrix
  mat4 view = lookAt(
      vec3(0, 0, 4), // Camera is at (0,0,4), in World Space
      vec3(0, 0, 0), // and looks at the origin
      vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
  );

  // Apply rotations to the view position. This wil get appleid to the whole scene
  view = rotate(view,
                -vx,
                vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
  view = rotate(view,
                -vy,
                vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
  view = rotate(view, -vz, vec3(0, 0, 1));

  // Define the light position and transform by the view matrix
  vec4 lightpos = view * vec4(light_x, light_y, light_z, 1.0);

  // Send our projection and view uniforms to the currently bound shader
  // I do that here because they are the same for all objects
  gl::Uniform1ui(colourmodeID, colourmode);
  gl::UniformMatrix4fv(viewID, 1, gl::FALSE_, &view[0][0]);
  gl::UniformMatrix4fv(projectionID, 1, gl::FALSE_, &projection[0][0]);
  gl::Uniform4fv(lightposID, 1, value_ptr(lightpos));

  /* Draw a small sphere in the lightsource position to visually represent the light source */
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(light_x, light_y, light_z));
    model.top() =
        scale(model.top(), vec3(0.05f, 0.05f, 0.05f)); // make a small sphere
    // Recalculate the normal matrix and send the model and normal matrices to the vertex shader																							// Recalculate the normal matrix and send to the vertex shader																								// Recalculate the normal matrix and send to the vertex shader																								// Recalculate the normal matrix and send to the vertex shader																						// Recalculate the normal matrix and send to the vertex shader
    gl::UniformMatrix4fv(modelID, 1, gl::FALSE_, &(model.top()[0][0]));
    normalmatrix = guts::InverseTranspose(mat3(view * model.top()));
    gl::UniformMatrix3fv(normalmatrixID, 1, gl::FALSE_, &normalmatrix[0][0]);

    /* Draw our lightposition sphere  with emit mode on*/
    emitmode = 1;
    gl::Uniform1ui(emitmodeID, emitmode);
    aSphere->Render(static_cast<guts::GLRenderMode>(drawmode));
    emitmode = 0;
    gl::Uniform1ui(emitmodeID, emitmode);
  }
  model.pop();

  // Define the global model transformations (rotate and scale). Note, we're not modifying thel ight source position
  model.top() = scale(model.top(),
                      vec3(model_scale, model_scale, model_scale));//scale equally in all axis
  model.top() = rotate(model.top(),
                       -angle_x,
                       glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
  model.top() = rotate(model.top(),
                       -angle_y,
                       glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
  model.top() = rotate(model.top(),
                       -angle_z,
                       glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis

  // This block of code draws the cube
  model.push(model.top());
  {
    // Define the model transformations for the cube
    model.top() = translate(model.top(), vec3(x + 0.5f, y, z));

    // Send the model uniform and normal matrix to the currently bound shader,
    gl::UniformMatrix4fv(modelID, 1, gl::FALSE_, &(model.top()[0][0]));

    // Recalculate the normal matrix and send to the vertex shader
    normalmatrix = guts::InverseTranspose(mat3(view * model.top()));
    gl::UniformMatrix3fv(normalmatrixID, 1, gl::FALSE_, &normalmatrix[0][0]);

    /* Draw our cube*/
    aCube->Render(static_cast<guts::GLRenderMode>(drawmode));
  }
  model.pop();

  // This block of code draws the sphere
  model.push(model.top());
  {
    model.top() = translate(model.top(), vec3(-x - 0.5f, 0, 0));
    model.top() = scale(model.top(),
                        vec3(model_scale / 3.f,
                             model_scale / 3.f,
                             model_scale / 3.f));//scale equally in all axis

    // Recalculate the normal matrix and send the model and normal matrices to the vertex shader																							// Recalculate the normal matrix and send to the vertex shader																								// Recalculate the normal matrix and send to the vertex shader																								// Recalculate the normal matrix and send to the vertex shader																						// Recalculate the normal matrix and send to the vertex shader
    gl::UniformMatrix4fv(modelID, 1, gl::FALSE_, &(model.top()[0][0]));
    normalmatrix = guts::InverseTranspose(mat3(view * model.top()));
    gl::UniformMatrix3fv(normalmatrixID, 1, gl::FALSE_, &normalmatrix[0][0]);

    aSphere->Render(static_cast<guts::GLRenderMode>(drawmode)); // Draw our sphere
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
  if (key == '1') light_x -= 0.05f;
  if (key == '2') light_x += 0.05f;
  if (key == '3') light_y -= 0.05f;
  if (key == '4') light_y += 0.05f;
  if (key == '5') light_z -= 0.05f;
  if (key == '6') light_z += 0.05f;
  if (key == '7') vx -= 1.f;
  if (key == '8') vx += 1.f;
  if (key == '9') vy -= 1.f;
  if (key == '0') vy += 1.f;
  if (key == 'O') vz -= 1.f;
  if (key == 'P') vz += 1.f;

  if (key == 'M' && action != GLFW_PRESS) {
    colourmode = !colourmode;
    cout << "colourmode=" << colourmode << endl;
  }

  /* Cycle between drawing vertices, mesh and filled polygons */
  if (key == ',' && action != GLFW_PRESS) {
    drawmode++;
    if (drawmode > 2) drawmode = 0;
  }
}

/* Entry point of program */
int main(int argc, char *argv[]) {
  guts::GlfwWindow
      *glw = new guts::GlfwWindow(1024, 768, "Position light example");

  glw->SetRenderer(display);
  glw->SetKeyCallback(keyCallback);
  glw->SetReshapeCallback(reshape);

  init(glw);

  glw->EventLoop();

  delete (glw);
  return 0;
}

