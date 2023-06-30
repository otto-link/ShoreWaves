// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#pragma once
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include <GLFW/glfw3.h>

class Array
{
public:
  std::vector<int>   shape;
  std::vector<float> vector;

  Array(std::vector<int> shape)
  {
    this->set_shape(shape);
  }

  inline std::vector<int> get_shape()
  {
    return shape;
  }

  std::vector<float> get_vector()
  {
    return this->vector;
  }

  void set_shape(std::vector<int> new_shape)
  {
    this->shape = new_shape;
    this->vector.resize(this->shape[0] * this->shape[1]);
  }

  float &operator()(int i, int j)
  {
    return this->vector[i * this->shape[1] + j];
  }

  const float &operator()(int i, int j) const ///< @overload
  {
    return this->vector[i * this->shape[1] + j];
  }

  float max() const
  {
    return *std::max_element(this->vector.begin(), this->vector.end());
  }

  float min() const
  {
    return *std::min_element(this->vector.begin(), this->vector.end());
  }

  void randomize(float a, float b, uint seed)
  {
    std::mt19937                          gen(seed);
    std::uniform_real_distribution<float> dis(a, b);
    for (auto &v : this->vector)
      v = dis(gen);
  }

  std::vector<uint8_t> to_img_8bit_grayscale()
  {
    std::vector<uint8_t> data(this->shape[0] * this->shape[1]);
    const float          vmax = this->max();
    const float          vmin = this->min();

    if (vmax > 0) // export a black image if not
    {
      // reorganize things to get an image with (i, j) used as (x, y)
      // coordinates, i.e. with (0, 0) at the bottom left
      float a = 1.f / (vmax - vmin);
      float b = -vmin / (vmax - vmin);
      int   k = 0;

      for (int j = this->shape[1] - 1; j > -1; j--)
        for (int i = 0; i < this->shape[0]; i++)
        {
          float   v = a * (*this)(i, j) + b;
          uint8_t c = (uint8_t)std::floor(255 * v);
          data[k++] = c;
        }
    }
    return data;
  }

  void to_texture(GLuint &image_texture, int colormap)
  {
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

    switch (colormap)
    {
    case 0:
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RGBA,
                   this->shape[0],
                   this->shape[1],
                   0,
                   GL_LUMINANCE,
                   GL_UNSIGNED_BYTE,
                   this->to_img_8bit_grayscale().data());
      break;
    }
  }
};
