/*
 Point Sprites
 Basic example to show how a simple particle animation
 with point sprites.
 Use SOIL to load an image into a texture.
 Addapted from Example 6.1 in the Redbook V4.3
 Iain Martin October 2014

 Adapted by Robert T 2017 to use GUTS instead.
*/

#include <memory>
#include <vector>
#include <glload/gl_4_1.hpp>
#include <guts/guts.h>

/* Define buffer object indices */
GLuint quad_vbo, quad_normals, quad_colours, quad_tex_coords;

/* Define textureID*/
//GLuint texID;
std::unique_ptr<guts::GLTexture> planeTex;

GLuint program;        /* Identifier for the shader prgoram */
GLuint vao;            /* Vertex array (Containor) object. This is the index of the VAO that will be the container for
					   our buffer objects */

GLuint colourmode;    /* Index of a uniform to switch the colour mode in the vertex shader
					  I've included this to show you how to pass in an unsigned integer into
					  your vertex shader. */

/* Position and view globals */
GLfloat angle_x, angle_inc_x, x, model_scale, z, y, vx, vy, vz;;
GLfloat angle_y, angle_inc_y, angle_z, angle_inc_z;
guts::GLRenderMode drawmode = guts::RENDER_NORMAL;

/* Uniforms*/
GLuint modelID, viewID, projectionID;
GLuint colourmodeID;
GLfloat point_sizeID;

GLfloat aspect_ratio;

struct PointsMetadata {
  glm::vec3 velocity;
  float speed, max_dist;
};

/* Point sprite object and adjustable parameters */
std::unique_ptr<guts::GLParticle<PointsMetadata>> particle_system;
GLfloat speed;
GLfloat maxdist;
GLfloat point_size;        // Used to adjust point size in the vertex shader

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper *glw) {
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
  vx = 0;
  vx = 0, vz = 0.f;

  /* Define the Blending function */
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Generate index (name) for one vertex array object
  glGenVertexArrays(1, &vao);

  // Create the vertex array object and make it current
  glBindVertexArray(vao);

  /* Load and build the vertex and fragment shaders */
  try {
    /* To show the analytic version, change file name of the fragment shader
       to "point_sprites_analytic.frag" */
    program = glw->LoadShader("point_sprites.vert", "point_sprites.frag");
//		program = glw->LoadShader("point_sprites.vert", "point_sprites_analytic.frag");
  }
  catch (exception &e) {
    cout << "Caught exception: " << e.what() << endl;
    cin.ignore();
    exit(0);
  }

  // Define uniforms to send to vertex shader
  modelID = glGetUniformLocation(program, "model");
  colourmodeID = glGetUniformLocation(program, "colourmode");
  point_sizeID = glGetUniformLocation(program, "size");
  viewID = glGetUniformLocation(program, "view");
  projectionID = glGetUniformLocation(program, "projection");

  // Create our quad and texture
  glGenBuffers(1, &quad_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

  try {
    /* Not actually needed if using one texture at a time */
    glActiveTexture(GL_TEXTURE0);

    /* load an image file directly as a new OpenGL texture */
    texID = SOIL_load_OGL_texture("..\\..\\images\\star.png",
                                  SOIL_LOAD_AUTO,
                                  SOIL_CREATE_NEW_ID,
                                  SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
                                      | SOIL_FLAG_NTSC_SAFE_RGB
                                      | SOIL_FLAG_COMPRESS_TO_DXT);

    /* check for an error during the load process */
    if (texID == 0) {
      printf("TexID SOIL loading error: '%s'\n", SOIL_last_result());
    }

    /* Standard bit of code to enable a uniform sampler for our texture */
    int loc = glGetUniformLocation(program, "tex1");
    if (loc >= 0) glUniform1i(loc, 0);
  }
  catch (exception &e) {
    printf("\nImage file loading failed.");
  }

  /* Define the texture behaviour parameters */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,
                  GL_TEXTURE_MAG_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

  // This is the location of the texture object (TEXTURE0), i.e. tex1 will be the name
  // of the sampler in the fragment shader
  int loc = glGetUniformLocation(program, "tex1");
  if (loc >= 0) glUniform1i(loc, 0);

  speed = 0.005f;
  maxdist = 0.6f;
  point_anim = new points2(10000, maxdist, speed);
  point_anim->create();
  point_size = 4;

  // Enable gl_PointSize
  glEnable(GL_PROGRAM_POINT_SIZE);
}

/* Called to update the display. Note that this function is called in the event loop in the wrapper
   class because we registered display as a callback function */
void display() {
  /* Define the background colour */
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  /* Clear the colour and frame buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Enable depth test  */
  glEnable(GL_DEPTH_TEST);

  /* Enable Blending for the analytic point sprite */
  glEnable(GL_BLEND);

  /* Make the compiled shader program current */
  glUseProgram(program);

  // Define the model transformations
  mat4 model = mat4(1.0f);
  model = translate(model, vec3(x, y, z));
  model = scale(model,
                vec3(model_scale * 3,
                     model_scale * 3,
                     model_scale * 3));//scale equally in all axis
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
  mat4 view = lookAt(
      vec3(0, 0, 4), // Camera is at (0,0,4), in World Space
      vec3(0, 0, 0), // and looks at the origin
      vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
  );

  // Apply rotations to the view position
  view = rotate(view,
                -vx,
                vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
  view = rotate(view,
                -vy,
                vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
  view = rotate(view,
                -vz,
                vec3(0, 0, 1)); //rotating in clockwise direction around z-axis

  // Send our uniforms variables to the currently bound shader,
  glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
  glUniform1ui(colourmodeID, colourmode);
  glUniform1f(point_sizeID, point_size);
  glUniformMatrix4fv(viewID, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projectionID, 1, GL_FALSE, &Projection[0][0]);

  /* Modify our animation variables */
  angle_x += angle_inc_x;
  angle_y += angle_inc_y;
  angle_z += angle_inc_z;

  point_anim->draw();
  point_anim->animate();
}

/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow *window, int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
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
    glfwSetWindowShouldClose(window, GL_TRUE);

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
  if (key == 'N' && action != GLFW_PRESS) {
    drawmode++;
    if (drawmode > 2) drawmode = 0;
  }

  /* Point sprite animation parameters */
  if (key == ',') point_size -= 1.f;
  if (key == '.') point_size += 1.f;
  if (key == 'O') {
    speed -= 0.0005f;
    if (speed < 0) speed = 0;
  }
  if (key == 'P') speed += 0.0005f;
  if (key == 'L') maxdist -= 0.1f;
  if (key == ';') maxdist += 0.1f;

  point_anim->updateParams(maxdist, speed);
}

/* Entry point of program */
int main(int argc, char *argv[]) {
  GLWrapper *glw = new GLWrapper(1024, 768, "Texture image example");;

  if (!ogl_LoadFunctions()) {
    fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
    return 0;
  }

  glw->setRenderer(display);
  glw->setKeyCallback(keyCallback);
  glw->setReshapeCallback(reshape);

  init(glw);

  glw->eventLoop();

  delete (glw);
  return 0;
}

