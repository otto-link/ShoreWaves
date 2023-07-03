// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#pragma once
#include <iostream>

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

  std::vector<uint8_t> to_img_8bit_grayscale();

  std::vector<uint8_t> to_img_8bit_rgb(Array *p_mask = nullptr);

  void to_texture(GLuint &image_texture, int colormap, Array *p_mask = nullptr);
};

Array distance_transform(const Array &array);
Array gradient_angle(const Array &array);
Array gradient_x(const Array &array);
Array gradient_y(const Array &array);
Array interp_nearest(const Array &x,
                     const Array &y,
                     const Array &z,
                     const Array &xi,
                     const Array &yi);
