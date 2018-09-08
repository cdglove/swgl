#include "swgl/image.hpp"
#include "swgl/model.hpp"
#include "swgl/pipeline.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include <GLFW/glfw3.h>

#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

class application {
 public:
  application()
      : rt_(width_, height_, swgl::image::RGB) {
    init_window_manager();
    init_imgui();
  }

  ~application() {
    cleanup_imgui();
    cleanup_window_manager();
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
      present();
    }

    return 0;
  }

 private:
  static void on_window_resized(GLFWwindow* window, int width, int height) {
    reinterpret_cast<application*>(glfwGetWindowUserPointer(window))
        ->handle_window_resized(width, height);
  }

  static void on_window_contents_scale_changed(
      GLFWwindow* window, float scalew, float scaleh) {
    reinterpret_cast<application*>(glfwGetWindowUserPointer(window))
        ->handle_window_contents_scale_changed(scalew, scaleh);
  }

  void init_window_manager() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    window_ = glfwCreateWindow(width_, height_, "swgl", nullptr, nullptr);
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, on_window_resized);
    glfwSwapInterval(1);
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glfwSetWindowContentScaleCallback(
        window_, on_window_contents_scale_changed);
    glfwGetWindowContentScale(window_, &scalew_, &scaleh_);
  }

  void cleanup_window_manager() {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  void init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL2_Init();
    ImGui::StyleColorsDark();
    float scale = std::max(scalew_, scaleh_);
    ImGui::GetStyle().ScaleAllSizes(scale);
    io.FontGlobalScale = scale;
  }

  void cleanup_imgui() {
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void present() const {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glRasterPos2d(-1, -1);

    // Because our image isn't 4 byte aligned at the start of each row.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glDrawPixels(width_, height_, GL_RGB, GL_UNSIGNED_BYTE, rt_.buffer());

    {
      static bool show_demo_window    = true;
      static bool show_another_window = false;
      static ImVec4 clear_color       = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

      // 2. Show a simple window that we create ourselves. We use a Begin/End
      // pair to created a named window.
      {
        static float f     = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!"
                                       // and append into it.

        ImGui::Text("This is some useful text."); // Display some text (you can
                                                  // use a format strings too)
        ImGui::Checkbox(
            "Demo Window", &show_demo_window); // Edit bools storing our window
                                               // open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat(
            "float", &f, 0.0f,
            1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3(
            "clear color",
            (float*)&clear_color); // Edit 3 floats representing a color

        if(ImGui::Button(
               "Button")) // Buttons return true when clicked (most widgets
                          // return true when edited/activated)
          counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text(
            "Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
      }

      // 3. Show another simple window.
      if(show_another_window) {
        ImGui::Begin(
            "Another Window",
            &show_another_window); // Pass a pointer to our bool variable (the
                                   // window will have a closing button that
                                   // will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if(ImGui::Button("Close Me"))
          show_another_window = false;
        ImGui::End();
      }

      ImGui::Render();
      ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }

    glfwSwapBuffers(window_);
  }

  void handle_window_resized(int width, int height) {
    width_  = width;
    height_ = height;
    rt_     = swgl::image(width, height, swgl::image::RGB);
    glViewport(0, 0, width_, height_);
  }

  void handle_window_contents_scale_changed(float scalew, float scaleh) {
    scalew_     = scalew;
    scaleh_     = scaleh;
    float scale = std::max(scalew_, scaleh_);
    ImGui::GetStyle().ScaleAllSizes(scale);
    ImGui::GetIO().FontGlobalScale = scale;
  }

  int width_    = 1600;
  int height_   = 1600;
  float scalew_ = 1.f;
  float scaleh_ = 1.f;
  swgl::image rt_;
  GLFWwindow* window_;
};

int main(int argc, char** argv) {
  application app;
  return app.run(argc, argv);
}
