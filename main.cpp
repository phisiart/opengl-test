#include <string>
#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>

void GlfwErrorCallback(int err, const char* str) {
  std::cerr << "Error: [" << err << "] " << str << std::endl;
}

// Don't need to change this.
// We want to draw 2 giant triangles that cover the whole screen.
struct Vertex {
  float x, y;
};
Vertex vertices[] = {
    { -1.f, -1.f },
    { 1.0f, -1.f },
    { 1.0f, 1.0f },
    { -1.f, -1.f },
    { -1.f, 1.0f },
    { 1.0f, 1.0f },
};

// Don't need to change this.
// The vertex shader only needs to take in the triangle points.
// No need for point transformations.
static const char* vertex_shader_text =
    "attribute vec2 point; // input to vertex shader\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(point, 0.0, 1.0);\n"
    "}\n";

// This is the main part.
static const char* fragment_shader_text =
    "uniform int width;\n"
    "uniform int height;\n"
    "void main()\n"
    "{\n"
    "    // TODO(zhixunt): Calculate pixel index.\n"
    "    gl_FragColor = vec4(gl_FragCoord.x / float(width), gl_FragCoord.y / float(height), 0.0, 1.0)\n;"
    "}\n";

int main(int argc, char *argv[]) {
  std::cout << "Hello, World!!" << std::endl;

  // Set an error handler.
  // This can be called before glfwInit().
  glfwSetErrorCallback(&GlfwErrorCallback);

  // Initialize GLFW.
  if (glfwInit() != GLFW_TRUE) {
    std::cout << "glfwInit() failed!" << std::endl;
    return 1;
  }

  GLint width = 640;
  GLint height = 480;

  // Create a window.
  // TODO(zhixunt): GLFW allows us to create an invisible window.
  // TODO(zhixunt): On retina display, window size is different from framebuffer size.
  GLFWwindow *window = glfwCreateWindow(width, height, "My Title", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "glfwCreateWindow() failed!" << std::endl;
    return 1;
  }

  // Before using any OpenGL API, we must specify a context.
  glfwMakeContextCurrent(window);

  // Create the vertex shader.
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
  glCompileShader(vertex_shader);

  // Create the fragment shader.
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
  glCompileShader(fragment_shader);

  // Combine the vertex and fragment shaders to create a "program".
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glUseProgram(program);

  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  auto point_attrib = static_cast<GLuint>(glGetAttribLocation(program, "point"));
  glEnableVertexAttribArray(point_attrib);
  glVertexAttribPointer(point_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

  glfwGetFramebufferSize(window, &width, &height);
  auto width_uniform = glGetUniformLocation(program, "width");
  auto height_uniform = glGetUniformLocation(program, "height");
  glUniform1i(width_uniform, width);
  glUniform1i(height_uniform, height);
  glViewport(0, 0, width, height);

  glClear(GL_COLOR_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glfwSwapBuffers(window);

  while (glfwWindowShouldClose(window) == GLFW_FALSE) {
    glfwPollEvents();
  }

  // Paired with glfwCreateWindow().
  glfwDestroyWindow(window);

  // Paired with glfwInit().
  glfwTerminate();

  return 0;
}
