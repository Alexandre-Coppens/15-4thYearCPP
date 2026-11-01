#include <chrono>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

constexpr int kWidth = 80;
constexpr int kCells = kWidth * kWidth;
constexpr int kGenerations = 64;
constexpr int kSamples = 50;

int at(int x, int y) { return y * kWidth + x; }

bool alive(const float* grid, int x, int y) {
  if (x < 0 || y < 0 || x >= kWidth || y >= kWidth) {
    return false;
  }
  return grid[at(x, y)] > 0.5f;
}

void step_scalar(const float* src, float* dst) {
  for (int y = 0; y < kWidth; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      int neighbors = 0;
      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          if (dx == 0 && dy == 0) {
            continue;
          }
          neighbors += alive(src, x + dx, y + dy);
        }
      }
      const bool cell = alive(src, x, y);
      const bool next = (cell && (neighbors == 2 || neighbors == 3)) || (!cell && neighbors == 3);
      dst[at(x, y)] = next ? 1.f : 0.f;
    }
  }
}

// TODO: une génération. Chaque ligne se parcourt par blocs de 8 (un __m256).
//       Le voisin hors grille vaut 0 ; à l'intérieur, il vient de la cellule d'à côté.
//       Règle par masques de comparaison, pas un if par cellule.
void step_avx2(const float* src, float* dst) {
  (void)src;
  std::memset(dst, 0, sizeof(float) * static_cast<std::size_t>(kCells));
}

void seed(float* grid) {
  std::memset(grid, 0, sizeof(float) * static_cast<std::size_t>(kCells));
  // Planeur en haut à gauche, clignotant au centre.
  const int on[] = {at(1, 0), at(2, 1), at(0, 2), at(1, 2), at(2, 2), at(3, 4), at(4, 4), at(5, 4)};
  for (int i : on) {
    grid[i] = 1.f;
  }
}

using Clock = std::chrono::steady_clock;

double ms_since(Clock::time_point start) {
  return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
}

void run(void (*step)(const float*, float*), const float* start, float* out) {
  std::vector<float> a(start, start + kCells);
  std::vector<float> b(static_cast<std::size_t>(kCells));
  float* src = a.data();
  float* dst = b.data();
  for (int g = 0; g < kGenerations; ++g) {
    step(src, dst);
    float* tmp = src;
    src = dst;
    dst = tmp;
  }
  std::memcpy(out, src, sizeof(float) * static_cast<std::size_t>(kCells));
}

bool same(const float* got, const float* truth) {
  for (int i = 0; i < kCells; ++i) {
    if (got[i] != truth[i]) {
      return false;
    }
  }
  return true;
}

struct Stats {
  double mean = 0;
  double stddev = 0;
};

Stats summarize(const double* samples, int n) {
  double mean = 0;
  for (int i = 0; i < n; ++i) {
    mean += samples[i];
  }
  mean /= n;
  double var = 0;
  for (int i = 0; i < n; ++i) {
    const double d = samples[i] - mean;
    var += d * d;
  }
  return {mean, std::sqrt(var / (n - 1))};
}

}  // namespace

int main() {
  std::vector<float> start(static_cast<std::size_t>(kCells));
  seed(start.data());
  std::vector<float> truth(static_cast<std::size_t>(kCells));
  std::vector<float> got(static_cast<std::size_t>(kCells));

  double scalar_ms[kSamples]{};
  double avx2_ms[kSamples]{};
  for (int s = 0; s < kSamples; ++s) {
    const auto t0 = Clock::now();
    run(step_scalar, start.data(), truth.data());
    scalar_ms[s] = ms_since(t0);

    const auto t1 = Clock::now();
    run(step_avx2, start.data(), got.data());
    avx2_ms[s] = ms_since(t1);
  }
  const bool ok = same(got.data(), truth.data());
  const Stats scalar = summarize(scalar_ms, kSamples);
  const Stats avx2 = summarize(avx2_ms, kSamples);

  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(12) << "" << std::setw(14) << "moyenne" << std::setw(14) << "ecart-type"
            << "\n";
  std::cout << std::setw(12) << "scalaire" << std::setw(14) << scalar.mean << std::setw(14)
            << scalar.stddev << "\n";
  std::cout << std::setw(12) << "avx2";
  if (ok) {
    std::cout << std::setw(14) << avx2.mean << std::setw(14) << avx2.stddev << "\n";
  } else {
    std::cout << std::setw(14) << "FAIL" << "\n";
  }

  return ok ? 0 : 1;
}
