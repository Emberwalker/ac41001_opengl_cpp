/*
 Basic example to demonstrate using a GLFW wrapper
 class with an OpenGL 4.x examples
*/

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include "guts/wrapper_glfw.h"
#include <iostream>

GLuint positionBufferObject;
GLuint program;
GLuint vao;

using namespace std;

/*
This function is called before entering the main rendering loop.
Use it for all you initialisation stuff
*/
void init(GLWrapper *glw)
{
	gl::GenVertexArrays(1, &vao);
	gl::BindVertexArray(vao);

	float vertexPositions[] = {
		0.75f, 0.75f, 0.0f, 1.0f,
		0.75f, -0.75f, 0.0f, 1.0f,
		-0.75f, -0.75f, 0.0f, 1.0f,
	};

	gl::GenBuffers(1, &positionBufferObject);
	gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferObject);
	gl::BufferData(gl::ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, gl::STATIC_DRAW);
	gl::BindBuffer(gl::ARRAY_BUFFER, 0);

	try
	{
		program = glw->LoadShader("basic.vert", "basic.frag");
	}
	catch (exception &e)
	{
		cout << "Caught exception: " << e.what() << endl;
		cin.ignore();
		exit(0);
	}

	glw->DisplayVersion();

}

//Called to update the display.
//You should call glfwSwapBuffers() after all of your rendering to display what you rendered.
void display()
{
	gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	gl::Clear(gl::COLOR_BUFFER_BIT);

	gl::UseProgram(program);

	gl::BindBuffer(gl::ARRAY_BUFFER, positionBufferObject);
	gl::EnableVertexAttribArray(0);
	gl::VertexAttribPointer(0, 4, gl::FLOAT, gl::FALSE_, 0, 0);

	gl::DrawArrays(gl::TRIANGLES, 0, 3);

	gl::DisableVertexAttribArray(0);
	gl::UseProgram(0);
}


/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	gl::Viewport(0, 0, (GLsizei)w, (GLsizei)h);
}

/* change view angle, exit upon ESC */
static void keyCallback(GLFWwindow* window, int k, int s, int action, int mods)
{
	if (action != GLFW_PRESS) return;

	cout << "KEY: " << (char)k << endl;

	if (k == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, gl::TRUE_);
}

/* An error callback function to output GLFW errors*/
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper *glw = new GLWrapper(1024, 768, "Hello Graphics World");

	glw->setRenderer(display);
	glw->setKeyCallback(keyCallback);
	glw->setReshapeCallback(reshape);
	glw->setErrorCallback(error_callback);

	init(glw);

	glw->eventLoop();

	delete(glw);
	return 0;
}



