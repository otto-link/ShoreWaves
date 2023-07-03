// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "Interpolate.hpp"

#include "core/array.hpp"

class GerstnerWave
{
public:
  std::vector<int> shape;
  Array           *p_h = nullptr;
  Array            dz = Array({0, 0});
  Array            x0 = Array({0, 0});
  Array            y0 = Array({0, 0});
  float            kinf = 4.f;
  float            alpha = 15.f / 180.f * M_PI;
  float            steepness = 0.6f;
  float            phi0 = 0.f;
  float            phase_speed = 1.f;
  float            kludge = 20.f;
  float            k_clipping_ratio = 4.f;
  float            shore_dist_ratio = 0.8f;
  float            shore_r_ratio = 0.9f;

  GerstnerWave(Array &h)
  {
    this->shape = h.shape;
    this->p_h = &h;
    this->update();
  }

  void update();

  void generate(float t);

  // private:
  float r;
  float omega;
  Array phi_depth = Array({0, 0});
  Array shore_dist = Array({0, 0});
};

class WaterDepth
{
public:
  std::vector<int> shape;
  Array            h = Array({0, 0});

  // fbm parameters
  std::vector<float> kw = {1.f, 4.f};
  uint               seed = 1;
  int                octaves = 3;
  float              weight = 0.2f;
  float              persistence = 0.5f;
  float              lacunarity = 2.f;

  // slope parameters
  float slope = 2.8f;
  float offset = -0.5f;
  float scaling = 0.4f;

  WaterDepth(std::vector<int> shape) : shape(shape)
  {
    this->h.set_shape(shape);
    this->update();
  }

  void set_shape(std::vector<int> new_shape)
  {
    this->shape = new_shape;
    this->h.set_shape(this->shape);
  }

  void update();
};
