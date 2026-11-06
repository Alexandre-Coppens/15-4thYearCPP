#pragma once
#include <vector>
struct Particle {
  alignas(16) float posType[4];
  alignas(16) float vel[4];
};
std::vector<float> boxSize();
float* boxPtr();
void initParticles(std::vector<Particle>& dst, int n);
void initBoids(std::vector<Particle>& dst, int n, float L);
