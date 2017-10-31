#include <iostream>
#include <GLFW/glfw3.h>

void GlfwErrorCallback(int err, const char* str)
{
  std::cerr << "Error: [" << err << "] " << str << std::endl;
}

struct Vertex {
  float x, y;
  float r, g, b;
};

Vertex vertices[3] = {
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
    "uniform mat4 MVP;\n"
    "attribute vec3 vCol;\n"
    "attribute vec2 vPos;\n"
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    color = vCol;\n"
    "}\n";

static const char* fragment_shader_text =
    "varying vec3 color;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
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

  // Create a window.
  GLFWwindow *window = glfwCreateWindow(640, 480, "My Title", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "glfwCreateWindow() failed!" << std::endl;
    return 1;
  }

  // Before using any OpenGL API, we must specify a context.
  glfwMakeContextCurrent(window);

  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
  glCompileShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
  glCompileShader(fragment_shader);

  GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  auto mvp_location = static_cast<GLuint>(glGetUniformLocation(program, "MVP"));
  auto vpos_location = static_cast<GLuint>(glGetAttribLocation(program, "vPos"));
  auto vcol_location = static_cast<GLuint>(glGetAttribLocation(program, "vCol"));

  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(float) * 5, (void*) 0);
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(float) * 5, (void*) (sizeof(float) * 2));

  while (glfwWindowShouldClose(window) == GLFW_FALSE) {
    int width, height;

    GLfloat mvp[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }

  // Paired with glfwCreateWindow().
  glfwDestroyWindow(window);

  // Paired with glfwInit().
  glfwTerminate();

  return 0;
}
