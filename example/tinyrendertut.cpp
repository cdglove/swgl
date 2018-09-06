#include "swgl/image.hpp"
#include "swgl/model.hpp"
#include "swgl/pipeline.hpp"
#include <glfw/glfw3.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

struct application {
  int width_  = 800;
  int height_ = 800;
  swgl::image rt_;

  application()
      : rt_(width_, height_, swgl::image::RGB) {
  }

  static void on_window_resized(GLFWwindow* window, int width, int height) {
    application* this_ =
        reinterpret_cast<application*>(glfwGetWindowUserPointer(window));
    this_->width_  = width;
    this_->height_ = height;
    this_->rt_     = swgl::image(width, height, swgl::image::RGB);
  }
};

int main(int argc, char** argv) {
  char const* file = nullptr;

  if(2 == argc) {
    file = argv[1];
  }
  else {
    std::cout << "Usage: tinyrendertut <obj>" << std::endl;
    return 0;
  }

  std::ifstream fin(file);
  if(!fin.is_open()) {
    std::cerr << "can't open file " << file << "\n";
    return -1;
  }

  application app;

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  GLFWwindow* window =
      glfwCreateWindow(app.width_, app.height_, "swgl", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, &app);
  glfwSetWindowSizeCallback(window, application::on_window_resized);
  glfwSwapInterval(1);
  glClearColor(0.f, 0.f, 0.f, 0.f);

  swgl::model model(fin);
  swgl::pipeline p;

  while(!glfwWindowShouldClose(window)) {
    app.rt_.clear();
    p.draw(model, app.rt_);

    glRasterPos2d(-1, -1);
    glDrawPixels(
        app.width_, app.height_, GL_RGB, GL_UNSIGNED_BYTE, app.rt_.buffer());
    //glFlush();
    glfwSwapBuffers(window);
    glfwPollEvents();

    static volatile int g_DumpTga = false;
    if(g_DumpTga) {
      app.rt_.write_tga_file("output.tga");
      g_DumpTga = false;
    }
  }

  glfwTerminate();
  return 0;
}
