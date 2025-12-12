#include <iostream>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

enum MentalRendererType {
  MENTAL_RENDERER_TYPE_OPENGL = 0,
  MENTAL_RENDERER_TYPE_VULKAN = 1,
};

int main(int argc, const char* argv[]) {
  spdlog::info("Hello, World!");
  return 0;
}