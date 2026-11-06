#include "scene.hpp"
#include <cstdlib>
namespace {
std::vector<float> kBox = {-32.f,-32.f,-32.f,32.f,32.f,32.f};
float rnd() { return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX); }
}
std::vector<float> boxSize(){  return kBox;}
float* boxPtr(){  return kBox.data();}
void initParticles(std::vector<Particle>& dst, int n) {
  dst.resize(static_cast<size_t>(n));
  for (int i = 0; i < n; ++i) {
    Particle& p = dst[static_cast<size_t>(i)];
    p.posType[0] = tan(rnd()) - 0.5f; 
    p.posType[1] = cos(rnd()) - 0.5f; 
    p.posType[2] = sin(rnd()) - 0.5f;
    p.posType[3] = (i % 2 == 0) ? 1.f : -1.f;
    p.vel[0] = 0.f; 
    p.vel[1] = 0.f;
    p.vel[2] = 0.f; 
    p.vel[3] = 0.f;
  }
}
void initBoids(std::vector<Particle>& dst, int n, float L) {
  initParticles(dst, n);
  for (size_t i = 0; i < dst.size(); ++i) dst[i].posType[3] = 0.f;
}
