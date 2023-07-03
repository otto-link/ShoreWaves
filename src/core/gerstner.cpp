// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#include "core/gerstner.hpp"
#include "core/array.hpp"
#include "core/fbm.hpp"

void GerstnerWave::update()
{
  this->shape = p_h->shape;
  this->r = this->steepness / this->kinf; // wave height
  this->omega = this->kinf * this->phase_speed;

  this->dz.set_shape(this->shape);

  // --- main grid

  this->x0.set_shape(this->shape);
  this->y0.set_shape(this->shape);

  for (int i = 0; i < this->shape[0]; i++)
  {
    float x = M_PI * (2.f * (float)i / (float)(this->shape[0] - 1) - 1.f);
    for (int j = 0; j < this->shape[1]; j++)
    {
      float y = M_PI * (2.f * (float)j / (float)(this->shape[1] - 1) - 1.f);
      this->x0(i, j) = x;
      this->y0(i, j) = y;
    }
  }

  // --- distance to the shore

  this->shore_dist.set_shape(this->shape);
  this->shore_dist = distance_transform(*this->p_h); // squared distance

  {
    float c_decay = 0.5f / std::pow((float)this->shape[0] / this->kinf *
                                        this->shore_dist_ratio,
                                    2.f);

    for (int i = 0; i < this->shape[0]; i++)
      for (int j = 0; j < this->shape[1]; j++)
        this->shore_dist(i, j) =
            1.f - std::exp(-this->shore_dist(i, j) * c_decay);
  }

  // --- accumulative phase lag due to depth variations

  this->phi_depth.set_shape(this->shape);

  // create rotated larger grid
  Array xr = Array(this->shape);
  Array yr = Array(this->shape);
  float ca = std::cos(this->alpha);
  float sa = std::sin(this->alpha);

  float scale = M_PI * M_SQRT2;

  for (int i = 0; i < this->shape[0]; i++)
  {
    float x = scale * (2.f * (float)i / (float)(this->shape[0] - 1) - 1.f);
    for (int j = 0; j < this->shape[1]; j++)
    {
      float y = scale * (2.f * (float)j / (float)(this->shape[1] - 1) - 1.f);
      xr(i, j) = x * ca - y * sa;
      yr(i, j) = x * sa + y * ca;
    }
  }

  // interpolate water depth on rotated grid
  Array hr = interp_nearest(this->x0, this->y0, *p_h, xr, yr);

  // compute accumulative phase lag
  Array fk = Array(this->shape);

  for (int i = 0; i < this->shape[0]; i++)
    for (int j = 0; j < this->shape[1]; j++)
    {
      float v = std::min(this->k_clipping_ratio,
                         1.f / std::sqrt(std::tanh(-this->kinf * hr(i, j))));
      fk(i, j) = v - 1.f;
      fk(i, j) *= this->kinf;
    }

  Array phi_depth_r = Array(this->shape);
  float dxr = xr(1, 0) - xr(0, 0);

  for (int j = 0; j < this->shape[1]; j++)
  {
    phi_depth_r(0, j) = dxr * fk(0, j);
    for (int i = 1; i < this->shape[0]; i++)
      phi_depth_r(i, j) = phi_depth_r(i - 1, j) + dxr * fk(i, j);
  }

  // interpolate back on initial mesh
  {
    Array x0r = Array(this->shape);
    Array y0r = Array(this->shape);

    // trick to have a regular grid
    for (int i = 0; i < this->shape[0]; i++)
    {
      float x = M_PI * (2.f * (float)i / (float)(this->shape[0] - 1) - 1.f);
      for (int j = 0; j < this->shape[1]; j++)
      {
        float y = M_PI * (2.f * (float)j / (float)(this->shape[1] - 1) - 1.f);
        xr(i, j) = M_SQRT2 * x;
        yr(i, j) = M_SQRT2 * y;
        x0r(i, j) = x * ca + y * sa;
        y0r(i, j) = -x * sa + y * ca;
      }
    }

    phi_depth = interp_nearest(xr, yr, phi_depth_r, x0r, y0r);
  }
}

void GerstnerWave::generate(float t)
{
  Array x = Array(this->shape);
  Array y = Array(this->shape);

  float ca = std::cos(this->alpha);
  float sa = std::sin(this->alpha);

  for (int i = 0; i < this->shape[0]; i++)
    for (int j = 0; j < this->shape[1]; j++)
    {
      float phi = this->kinf * ca * x0(i, j) + this->kinf * sa * y0(i, j) -
                  this->omega * t + this->phi_depth(i, j) + this->phi0;

      float rloc =
          this->r * (1.f - this->shore_r_ratio * this->shore_dist(i, j));
      rloc *= std::pow(this->shore_dist(i, j), 0.2f);

      x(i, j) = x0(i, j) - rloc * std::sin(phi) * ca;
      y(i, j) = y0(i, j) - rloc * std::sin(phi) * sa;
      dz(i, j) = -rloc * std::cos(phi);

      // kuldgeing
      float ck = (1.f - this->shore_dist(i, j)) * this->kludge;
      dz(i, j) = -rloc * std::cos(phi - ck * dz(i, j));
    }

  {
    _2D::BilinearInterpolator<float> interp;
    interp.setData(this->x0.vector, this->y0.vector, dz.vector);

    for (int i = 0; i < this->shape[0]; i++)
      for (int j = 0; j < this->shape[1]; j++)
      {
        if ((*this->p_h)(i, j) < 0.f)
          this->dz(i, j) = interp(x(i, j), y(i, j));
        else
          this->dz(i, j) = 0.f;
      }
  }
}

void WaterDepth::update()
{
  this->h = fbm_perlin(this->shape,
                       this->kw,
                       this->seed,
                       this->octaves,
                       this->weight,
                       this->persistence,
                       this->lacunarity);

  for (int i = 0; i < this->h.shape[0]; i++)
  {
    float dh =
        slope * (float)(i - 0.5f * this->h.shape[0]) / float(this->h.shape[0]);
    for (int j = 0; j < this->h.shape[1]; j++)
    {
      this->h(i, j) += dh + this->offset;
      this->h(i, j) *= this->scaling;
    }
  }
}
