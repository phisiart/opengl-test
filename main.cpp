#include <string>
#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h>

namespace gl {
inline const char* GLGetErrorString(GLenum error) {
  switch (error) {
    case GL_NO_ERROR:
      return "GL_NO_ERROR";
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
    default:
      return "Unknown OpenGL error code";
  }
}

}  // namespace gl

// TODO(zhixunt): When porting to TVM, change this to
//   CHECK(err == GL_NO_ERROR) << ...;
void OPENGL_CHECK_ERROR() {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    std::cerr << "OpenGL error, code=" << err << ": "
              << gl::GLGetErrorString(err);
    exit(1);
  }
}

/*!
 * \brief Protected OpenGL call.
 * \param func Expression to call.
 */
#define OPENGL_CALL(func)                                             \
  {                                                                   \
    (func);                                                           \
    OPENGL_CHECK_ERROR();                                             \
  }

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
  OPENGL_CALL(glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr));
  glCompileShader(vertex_shader);

  // Create the fragment shader.
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  OPENGL_CALL(glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr));
  OPENGL_CALL(glCompileShader(fragment_shader));

  // Combine the vertex and fragment shaders to create a "program".
  GLuint program = glCreateProgram();
  OPENGL_CALL(glAttachShader(program, vertex_shader));
  OPENGL_CALL(glAttachShader(program, fragment_shader));
  OPENGL_CALL(glLinkProgram(program));
  OPENGL_CALL(glUseProgram(program));

  GLuint vertex_buffer;
  OPENGL_CALL(glGenBuffers(1, &vertex_buffer));
  OPENGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
  OPENGL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

  auto point_attrib = static_cast<GLuint>(glGetAttribLocation(program, "point"));
  OPENGL_CALL(glEnableVertexAttribArray(point_attrib));
  OPENGL_CALL(glVertexAttribPointer(point_attrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr));

  glfwGetFramebufferSize(window, &width, &height);
  auto width_uniform = glGetUniformLocation(program, "width");
  auto height_uniform = glGetUniformLocation(program, "height");
  OPENGL_CALL(glUniform1i(width_uniform, width));
  OPENGL_CALL(glUniform1i(height_uniform, height));
  OPENGL_CALL(glViewport(0, 0, width, height));

  glClear(GL_COLOR_BUFFER_BIT);

  OPENGL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

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
