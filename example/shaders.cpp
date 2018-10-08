//
// swgl/examples/shaders.cpp
//
// Copyright (c) Chris Glover, 2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "swgl/camera.hpp"
#include "swgl/colour.hpp"
#include "swgl/image.hpp"
#include "swgl/model.hpp"
#include "swgl/pipeline.hpp"
#include "swgl/shaders/flat.hpp"
#include "swgl/shaders/gouraud.hpp"
#include "swgl/shaders/phong.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

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
    swgl::shaders::flat f;
    swgl::shaders::gouraud g;
    swgl::shaders::phong p;

    shader_names_ = "Flat\0Gouraud\0Phong\0\0";
    shaders_.push_back(&f);
    shaders_.push_back(&g);
    shaders_.push_back(&p);
    options_.shader = 2;

    f.set_depth(depth_);
    f.set_model(model);
    f.set_render_target(rt_);
    f.set_albedo(diffuse);

    g.set_depth(depth_);
    g.set_model(model);
    g.set_render_target(rt_);
    g.set_albedo(diffuse);

    p.set_depth(depth_);
    p.set_model(model);
    p.set_render_target(rt_);
    p.set_albedo(diffuse);

    swgl::shaders::basic_lighted_model::draw_info draw_data;

    while(!glfwWindowShouldClose(window_)) {
      clear_frame();
      update();
      fill_draw_data(draw_data);
      swgl::shaders::basic_lighted_model const* shader =
          shaders_[options_.shader];

      swgl::pipeline_counters frame_counters;
      frame_counters += shader->draw(draw_data);
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

  static swgl::vector3f polar_to_3d(float theta, float phi, float rad) {
    swgl::vector3f ret;
    float ctp = theta * 3.1415f * 2;
    float cpp = phi * 3.1415f * 2;

    ret.x = rad * std::sin(ctp) * std::cos(cpp);
    ret.y = rad * std::sin(ctp) * std::sin(cpp);
    ret.z = rad * std::cos(ctp);
    return ret;
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

    // Options window
    {
      ImGui::Begin("Options");

      ImGui::Combo(
          "Display Buffer", &options_.visualize_buffer, "Colour\0Depth\0\0");
      ImGui::Combo("Shader", &options_.shader, shader_names_);
      ImGui::Checkbox("Rotate", &options_.auto_rotate);

      ImGui::Separator();
      ImGui::Text("Camera");
      ImGui::SliderFloat("CDistance", &camera_radius_, 0.2f, 5.f);
      ImGui::SliderFloat2("CThetaPhi", camera_theta_phi_.raw.data(), -1.f, 1.f);

      ImGui::Separator();
      ImGui::Text("Light Direction");
      ImGui::SliderFloat("LDistance", &light_distance_, 0.2f, 5.f);
      ImGui::SliderFloat2("LThetaPhi", light_theta_phi_.raw.data(), -1.f, 1.f);

      ImGui::ColorEdit3(
          "Clear Colour",
          options_.clear_colour.data()); // Edit 3 floats representing a color

      ImGui::End();
    }

    // Stats window
    {
      ImGui::Begin("Draw Stats");

      ImGui::Text("pixels = %d", frame_stats.pixel_count());
      ImGui::Text("triangles = %d", frame_stats.triangle_count());
      ImGui::Text("draws = %d", frame_stats.draw_count());

      ImGui::Text(
          "Application average %.3f ms/frame (%.1f FPS)",
          1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

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

  void clear_frame() {
    rt_.clear(swgl::colour_cast<std::uint8_t>(options_.clear_colour));
    std::fill(depth_.begin(), depth_.end(), -std::numeric_limits<float>::max());
  }

  void update() {
    if(options_.auto_rotate) {
      auto time        = std::chrono::steady_clock::now().time_since_epoch();
      swgl::vector3f x = polar_to_3d(
          std::chrono::duration<float>(time).count() * 0.1f, 0.f, 1.f);
      swgl::vector3f z = cross(x, swgl::vector3f(0.f, 1.f, 0.f));
      swgl::matrix4f m(swgl::init::identity);
      m.set_row(0, swgl::vector_widen<4>(x, 0.f));
      m.set_row(1, swgl::vector4f(0.f, 1.f, 0.f, 0.f));
      m.set_row(2, swgl::vector_widen<4>(z, 0.f));
      model_ = m;
    }
  }

  void fill_draw_data(
      swgl::shaders::basic_lighted_model::draw_info& draw_data) {
    draw_data.eye        = get_eye();
    draw_data.projection = get_projection();
    draw_data.view       = get_view(draw_data.eye);
    draw_data.viewport = swgl::viewport_matrix(0, 0, rt_.width(), rt_.height());
    draw_data.ambient_light = 0.2f;
    draw_data.directional_light =
        polar_to_3d(light_theta_phi_[0], light_theta_phi_[1], 1.f);
    draw_data.point_light =
        -polar_to_3d(light_theta_phi_[0], light_theta_phi_[1], light_distance_);

    draw_data.model = model_;
  }

  swgl::vector3f get_eye() {
    return polar_to_3d(
        camera_theta_phi_[0], camera_theta_phi_[1], camera_radius_);
  }

  swgl::matrix4f get_view(swgl::vector3f const& eye) const {
    auto view = swgl::lookat(
        eye, swgl::vector3f::zero(), swgl::vector3f(0.f, 1.f, 0.f));
    view.set_column(3, view.get_column(3) * camera_radius_);
    return view;
  }

  swgl::matrix4f get_projection() const {
    swgl::matrix4f proj = swgl::matrix4f::identity();
    proj[3][2]          = -1.f / camera_radius_;
    return proj;
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

  int width_           = 800;
  int height_          = 800;
  float scalew_        = 1.f;
  float scaleh_        = 1.f;
  float camera_radius_ = 1.5f;
  swgl::vector2f camera_theta_phi_{0.08f, 0.4f};
  swgl::vector2f light_theta_phi_;
  float light_distance_ = 1.f;
  swgl::vector3f eye_;
  swgl::image rt_;
  swgl::matrix4f camera_ = swgl::matrix4f::identity();
  swgl::matrix4f model_  = swgl::matrix4f::identity();
  std::vector<float> depth_;
  char const* shader_names_ = nullptr;
  std::vector<swgl::shaders::basic_lighted_model*> shaders_;
  GLFWwindow* window_;

  struct options {
    swgl::colour<float> clear_colour{0, 0, 0, 1};
    int visualize_buffer = 0;
    int shader           = 0;
    bool auto_rotate     = true;
  } options_;
};

int main(int argc, char** argv) {
  application app;
  return app.run(argc, argv);
}
