#include "swgl/colour.hpp"
#include "swgl/image.hpp"
#include "swgl/model.hpp"
#include "swgl/pipeline.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <fstream>
#include <string>

const swgl::colour<std::uint8_t> white =
    swgl::colour<std::uint8_t>(255, 255, 255, 255);
const swgl::colour<std::uint8_t> red =
    swgl::colour<std::uint8_t>(255, 0, 0, 255);
const swgl::colour<std::uint8_t> green =
    swgl::colour<std::uint8_t>(0, 255, 0, 255);

static std::vector<float> get_normalised_depth(
    std::vector<float> const& depth) {
  float min = std::numeric_limits<float>::max();
  float max = -std::numeric_limits<float>::max();
  for(float f : depth) {
    if(f != -std::numeric_limits<float>::max()) {
      min = std::min(f, min);
      max = std::max(f, max);
    }
  }
  float rng = max - min;
  std::vector<float> depth_copy;
  if(rng > 0.001f) {
    depth_copy.resize(depth.size());
    float rng_recip = 1.f / rng;
    std::transform(
        depth.begin(), depth.end(), depth_copy.begin(),
        [min, rng_recip](float sample) { return (sample - min) * rng_recip; });
    return depth_copy;
  }

  depth_copy = depth;
  return depth_copy;
}

static swgl::model load_model(char const* filename) {
  std::ifstream fin(filename);
  if(!fin.is_open()) {
    std::cerr << "can't open file " << filename << "\n";
    throw std::runtime_error("filed to open file.");
  }
  return swgl::model(fin);
}

static swgl::image load_texture(std::string filename, char const* suffix) {
  swgl::image dest;
  size_t dot = filename.find_last_of(".");
  if(dot != std::string::npos) {
    filename.erase(filename.begin() + dot, filename.end());
    filename.append(suffix);
    std::cerr << "texture file " << filename << " loading "
              << (dest.read_tga_file(filename.c_str()) ? "ok" : "failed")
              << std::endl;
    dest.flip_vertically();
  }
  return dest;
}

class application {
 public:
  application()
      : rt_(width_, height_, swgl::image::RGB)
      , depth_(width_ * height_) {
    init_window_manager();
    init_imgui();
  }

  ~application() {
    cleanup_imgui();
    cleanup_window_manager();
  }

  int run(int argc, char** argv) {

    char const* file = nullptr;
    if(2 != argc) {
      std::cout << "Usage: tinyrendertut <obj>" << std::endl;
      return 0;
    }

    file = argv[1];

    swgl::model model   = load_model(file);
    swgl::image diffuse = load_texture(file, "_diffuse.tga");
    swgl::pipeline p;
    p.set_depth(depth_);
    p.set_model(model);
    p.set_render_target(rt_);
    p.set_texture(0, diffuse);
    while(!glfwWindowShouldClose(window_)) {
      rt_.clear(swgl::colour_cast<std::uint8_t>(options_.clear_colour));
      std::fill(
          depth_.begin(), depth_.end(), -std::numeric_limits<float>::max());
      swgl::pipeline_counters frame_counters;
      frame_counters += p.draw();

      //   for(int i = 0; i < rt_.width(); ++i) {
      //     for(int j = 0; j < rt_.height(); ++j) {
      //       swgl::image::colour_type c = rt_.get(i, j);
      //       for(int k = 0; k < 3; ++k) {
      // auto f = swgl::colour_cast<float>(c);
      //         f[k] = std::pow(f[k], 2.2f);
      // c = swgl::colour_cast<std::uint8_t>(f);
      //         rt_.set(i, j, c);
      //       }
      //     }
      //   }

      update_window_manager();
      update_imgui(frame_counters);
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
    width_  = static_cast<int>(width_ * scalew_);
    height_ = static_cast<int>(height_ * scaleh_);
    glfwSetWindowSize(window_, height_, width_);
  }

  void cleanup_window_manager() {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  void update_window_manager() {
    glfwPollEvents();
  }

  void init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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

  void update_imgui(swgl::pipeline_counters const& frame_stats) {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    static bool show_demo_window    = true;
    static bool show_another_window = false;

    // 2. Show a simple window that we create ourselves. We use a Begin/End
    // pair to created a named window.
    {
      static float f     = 0.0f;
      static int counter = 0;

      ImGui::Begin("Options"); // Create a window called "Hello, world!"
                               // and append into it.

      ImGui::Text("This is some useful text."); // Display some text (you can
                                                // use a format strings too)

      ImGui::Combo(
          "Display Buffer", &options_.visualize_buffer, "Colour\0Depth\0\0");
      ImGui::Checkbox("Another Window", &show_another_window);

      ImGui::SliderFloat(
          "float", &f, 0.0f,
          1.0f); // Edit 1 float using a slider from 0.0f to 1.0f

      ImGui::ColorEdit3(
          "clear color",
          options_.clear_colour.data()); // Edit 3 floats representing a color

      if(ImGui::Button("Button")) { // Buttons return true when clicked (most
        // widgets return true when edited/activated)
        counter++;
      }
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);
    }

    if(ImGui::CollapsingHeader("Draw Stats")) {
      ImGui::Indent();
      ImGui::Text("pixels = %d", frame_stats.pixel_count());
      ImGui::Text("triangles = %d", frame_stats.triangle_count());
      ImGui::Text("draws = %d", frame_stats.draw_count());
      ImGui::Unindent();
    }

    ImGui::Text(
        "Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    //    ImGui::ShowDemoWindow();

    ImGui::Render();
  }

  void present() const {
    glRasterPos2d(-1, -1);

    // Because our image isn't 4 byte aligned at the start of each row.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);

    switch(options_.visualize_buffer) {
    case 0:
    default:
      glDrawPixels(width_, height_, GL_RGB, GL_UNSIGNED_BYTE, rt_.data());
      break;
    case 1:
      std::vector<float> depth_norm = get_normalised_depth(depth_);
      glDrawPixels(width_, height_, GL_RED, GL_FLOAT, depth_norm.data());
      break;
    }
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_);
  }

  void handle_window_resized(int width, int height) {
    width_  = width;
    height_ = height;
    rt_     = swgl::image(width, height, swgl::image::RGB);
    depth_.resize(width * height);
    glViewport(0, 0, width_, height_);
  }

  void handle_window_contents_scale_changed(float scalew, float scaleh) {
    scalew_     = scalew;
    scaleh_     = scaleh;
    float scale = std::max(scalew_, scaleh_);
    ImGui::GetStyle().ScaleAllSizes(scale);
    ImGui::GetIO().FontGlobalScale = scale;
  }

  int width_    = 800;
  int height_   = 800;
  float scalew_ = 1.f;
  float scaleh_ = 1.f;
  swgl::image rt_;
  std::vector<float> depth_;
  GLFWwindow* window_;

  struct options {
    swgl::colour<float> clear_colour{0, 0, 0, 1};
    int visualize_buffer{0};
  } options_;
};

int main(int argc, char** argv) {
  application app;
  return app.run(argc, argv);
}
