/**
  wrapper_glfw.cpp
  Modified from the OpenGL GLFW example to provide a wrapper GLFW class
  and to include shader loader functions to include shaders as text files
  Iain Martin August 2014

  Heavily adapted by Robert T. 2017
  */

#include "wrapper_glfw.h"

/* Inlcude some standard headers */

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;
namespace fs = boost::filesystem;

/* Constructor for wrapper object */
GLWrapper::GLWrapper(int width, int height, char *title) {

    this->width = width;
    this->height = height;
    this->title = title;
    this->fps = 60;
    this->running = true;

    /* Initialise GLFW and exit if it fails */
    if (!glfwInit()) {
        cout << "Failed to initialize GLFW." << endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
    glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title, 0, 0);
    if (!window) {
        cout << "Could not open GLFW window." << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    /* Obtain an OpenGL context and assign to the just opened GLFW window */
    glfwMakeContextCurrent(window);

    /* Initialise GLLoad library. You must have obtained a current OpenGL */
    if (!glload::LoadFunctions()) {
        cerr << "LoadFunctions() failed. Exiting" << endl;
        glfwTerminate();
        return;
    }

    /* Can set the Window title at a later time if you wish*/
    glfwSetWindowTitle(window, "Hello Graphics (again)");

    glfwSetInputMode(window, GLFW_STICKY_KEYS, true);
}


/* Terminate GLFW on destruvtion of the wrapepr object */
GLWrapper::~GLWrapper() {
    glfwTerminate();
}

/* Returns the GLFW window handle, required to call GLFW functions outside this class */
GLFWwindow *GLWrapper::getWindow() {
    return window;
}


/*
 * Pront OpenGL Version
 */
void GLWrapper::DisplayVersion() {
    int major, minor;
    gl::GetIntegerv(gl::MAJOR_VERSION, &major);
    gl::GetIntegerv(gl::MAJOR_VERSION, &minor);
    cout << "OpenGL Version = " << major << "." << minor << endl;

}


/*
GLFW_Main function normally starts the windows system, calls any init routines
and then starts the event loop which runs until the program ends
*/
int GLWrapper::eventLoop() {
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Call function to draw your graphics
        renderer();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


/* Register an error callback function */
void GLWrapper::setErrorCallback(void(*func)(int error, const char *description)) {
    this->error_callback = func;
    glfwSetErrorCallback(func);
}

/* Register a display function that renders in the window */
void GLWrapper::setRenderer(void(*func)()) {
    this->renderer = func;
}

/* Register a callback that runs after the window gets resized */
void GLWrapper::setReshapeCallback(void(*func)(GLFWwindow *window, int w, int h)) {
    reshape = func;
    glfwSetFramebufferSizeCallback(window, reshape);
}


/* Register a callback to respond to keyboard events */
void GLWrapper::setKeyCallback(void(*func)(GLFWwindow *window, int key, int scancode, int action, int mods)) {
    keyCallBack = func;
    glfwSetKeyCallback(window, keyCallBack);
}


/* Build shaders from strings containing shader source code */
GLuint GLWrapper::BuildShader(GLenum eShaderType, const string &shaderText) {
    GLuint shader = gl::CreateShader(eShaderType);
    const char *strFileData = shaderText.c_str();
    gl::ShaderSource(shader, 1, &strFileData, NULL);

    gl::CompileShader(shader);

    GLint status;
    gl::GetShaderiv(shader, gl::COMPILE_STATUS, &status);
    if (status == gl::FALSE_) {
        // Output the compile errors

        GLint infoLogLength;
        gl::GetShaderiv(shader, gl::INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        gl::GetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        const char *strShaderType = NULL;
        switch (eShaderType) {
            case gl::VERTEX_SHADER:
                strShaderType = "vertex";
                break;
            case gl::GEOMETRY_SHADER:
                strShaderType = "geometry";
                break;
            case gl::FRAGMENT_SHADER:
                strShaderType = "fragment";
                break;
        }

        cerr << "Compile error in " << strShaderType << "\n\t" << strInfoLog << endl;
        delete[] strInfoLog;

        throw runtime_error("Shader compile exception");
    }

    return shader;
}

/* Read a text file into a string*/
string GLWrapper::readFile(std::string fname) {
    auto loc = fs::detail::current_path();
    cout << loc.string() << endl;
    loc /= fname;
    fs::ifstream fileStream(loc, ios::in);

    if (!fileStream.is_open()) {
        cerr << "Could not read file " << loc.string() << ". File does not exist." << endl;
        return "";
    }

    stringstream strm;
    strm << fileStream.rdbuf();
    auto content = strm.str();
    /*string line;
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }*/

    fileStream.close();
    return content;
}

/* Load vertex and fragment shader and return the compiled program */
GLuint GLWrapper::LoadShader(std::string vertex_path, std::string fragment_path) {
    GLuint vertShader, fragShader;

    // Read shaders
    string vertShaderStr = readFile(vertex_path);
    string fragShaderStr = readFile(fragment_path);

    GLint result = gl::FALSE_;
    int logLength;

    vertShader = BuildShader(gl::VERTEX_SHADER, vertShaderStr);
    fragShader = BuildShader(gl::FRAGMENT_SHADER, fragShaderStr);

    cout << "Linking program" << endl;
    GLuint program = gl::CreateProgram();
    gl::AttachShader(program, vertShader);
    gl::AttachShader(program, fragShader);
    gl::LinkProgram(program);

    gl::GetProgramiv(program, gl::LINK_STATUS, &result);
    gl::GetProgramiv(program, gl::INFO_LOG_LENGTH, &logLength);
    vector<char> programError((logLength > 1) ? logLength : 1);
    gl::GetProgramInfoLog(program, logLength, NULL, &programError[0]);
    cout << &programError[0] << endl;

    gl::DeleteShader(vertShader);
    gl::DeleteShader(fragShader);

    return program;
}

/* Load vertex and fragment shader and return the compiled program */
GLuint GLWrapper::BuildShaderProgram(string vertShaderStr, string fragShaderStr) {
    GLuint vertShader, fragShader;
    GLint result = gl::FALSE_;

    try {
        vertShader = BuildShader(gl::VERTEX_SHADER, vertShaderStr);
        fragShader = BuildShader(gl::FRAGMENT_SHADER, fragShaderStr);
    }
    catch (exception &e) {
        cout << "Exception: " << e.what() << endl;
        throw runtime_error("BuildShaderProgram() Build shader failure. Abandoning");
    }

    GLuint program = gl::CreateProgram();
    gl::AttachShader(program, vertShader);
    gl::AttachShader(program, fragShader);
    gl::LinkProgram(program);

    GLint status;
    gl::GetProgramiv(program, gl::LINK_STATUS, &status);
    if (status == gl::FALSE_) {

        GLint infoLogLength;
        gl::GetProgramiv(program, gl::INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        gl::GetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        cerr << "Linker error: " << strInfoLog << endl;

        delete[] strInfoLog;
        throw runtime_error("Shader could not be linked.");
    }

    gl::DeleteShader(vertShader);
    gl::DeleteShader(fragShader);

    return program;
}