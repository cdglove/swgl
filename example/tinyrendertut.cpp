#include "swgl/pipeline.hpp"
#include "swgl/image.hpp"
#include "swgl/model.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width      = 800;
const int height     = 800;

int main(int argc, char** argv) {
  char const* file = nullptr;

  if(2 == argc) {
    file = argv[1];
  } else {
    file = "assets/african_head.obj";
  }

  std::ifstream fin(file);
  if(!fin.is_open()) {
    std::cerr << "can't open file " << file << "\n";
    return -1;
  }

  swgl::model model(fin);
  swgl::image rt(width, height, swgl::image::RGB);
  swgl::pipeline p;
  p.draw(model, rt);
  
  rt.flip_vertically(); // i want to have the origin at the left bottom
                        // corner of the image
  rt.write_tga_file("output.tga");
  return 0;
}
