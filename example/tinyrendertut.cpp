#include "swgl/image.hpp"
#include "swgl/model.hpp"
#include "swgl/pipeline.hpp"
#include <glfw/glfw3.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

class application {
 public:
  application()
      : rt_(width_, height_, swgl::image::RGB) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    window_ =
        glfwCreateWindow(width_, height_, "swgl", nullptr, nullptr);
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, on_window_resized);
    glfwSwapInterval(1);
    glClearColor(0.f, 0.f, 0.f, 0.f);
  }

  ~application() {
    glfwTerminate();
  }

  int run(int argc, char** argv) {
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
    swgl::model model(fin);
    swgl::pipeline p;

    while(!glfwWindowShouldClose(window_)) {
      rt_.clear();
      p.draw(model, rt_);

      glRasterPos2d(-1, -1);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glPixelStorei(GL_PACK_ROW_LENGTH, 0);
      glDrawPixels(width_, height_, GL_RGB, GL_UNSIGNED_BYTE, rt_.buffer());
      glfwSwapBuffers(window_);
      glfwPollEvents();

      static volatile int g_DumpTga = false;
      if(g_DumpTga) {
        rt_.write_tga_file("output.tga");
        g_DumpTga = false;
      }
    }

    return 0;
  }

  static void on_window_resized(GLFWwindow* window, int width, int height) {
    reinterpret_cast<application*>(glfwGetWindowUserPointer(window))
        ->handle_window_resized(width, height);
  }

 private:
  void handle_window_resized(int width, int height) {
    width_  = width;
    height_ = height;
    rt_     = swgl::image(width, height, swgl::image::RGB);
    glViewport(0, 0, width_, height_);
  }
  int width_  = 800;
  int height_ = 800;
  swgl::image rt_;
  GLFWwindow* window_;
};

int main(int argc, char** argv) {
  application app;
  return app.run(argc, argv);
}
