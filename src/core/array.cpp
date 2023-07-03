// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#include "core/array.hpp"

float f(int i, float gi)
{
  return (float)(i * i) + gi * gi;
}

int sep(int i, int u, float gi, float gu)
{
  return (int)((u * u - i * i + gu * gu - gi * gi) / (2 * (u - i)));
}

Array distance_transform(const Array &array)
{
  // A. Meijster, J. B. T. M. Roerdink, and W. H. Hesselink. A general
  // algorithm for computing distance transforms in linear time. In
  // Mathematical Morphology and its Applications to Image and Signal
  // Processing, pages 331–340. Kluwer Academic Publishers, 2000.

  Array dt = Array(array.shape); // output distance
  Array g = Array(array.shape);
  int   ni = array.shape[0];
  int   nj = array.shape[1];
  float inf = (float)(ni + nj);

  // phase 1
  for (int i = 0; i < ni; i++)
  {
    // scan 1
    if (array(i, 0) > 0.f)
      g(i, 0) = 0.f;
    else
      g(i, 0) = inf;

    for (int j = 1; j < nj; j++)
    {
      if (array(i, j) > 0.f)
        g(i, j) = 0.f;
      else
        g(i, j) = 1.f + g(i, j - 1);
    }

    // scan 2
    for (int j = nj - 2; j > -1; j--)
      if (g(i, j + 1) < g(i, j))
        g(i, j) = 1.f + g(i, j + 1);
  }

  // phase 2
  std::vector<int> s(std::max(ni, nj));
  std::vector<int> t(std::max(ni, nj));

  for (int j = 0; j < nj; j++)
  {
    int q = 0;
    s[0] = 0;
    t[0] = 0;

    // scan 3
    for (int u = 1; u < ni; u++)
    {
      while ((q >= 0) and (f(t[q] - s[q], g(s[q], j)) > f(t[q] - u, g(u, j))))
        q--;

      if (q < 0)
      {
        q = 0;
        s[0] = u;
      }
      else
      {
        int w = 1 + sep(s[q], u, g(s[q], j), g(u, j));

        if (w < ni)
        {
          q++;
          s[q] = u;
          t[q] = w;
        }
      }
    }

    // scan 4
    for (int u = ni - 1; u > -1; u--)
    {
      dt(u, j) = f(u - s[q], g(s[q], j));
      if (u == t[q])
        q--;
    }
  }

  return dt;
}

Array gradient_angle(const Array &array)
{
  Array dx = gradient_x(array);
  Array dy = gradient_y(array);
  Array alpha = Array(array.shape);

  std::transform(dx.vector.begin(),
                 dx.vector.end(),
                 dy.vector.begin(),
                 alpha.vector.begin(),
                 [](float a, float b) { return std::atan2(b, a); });

  return alpha;
}

Array gradient_x(const Array &array)
{
  Array dx = Array(array.shape);
  for (int i = 1; i < array.shape[0] - 1; i++)
    for (int j = 0; j < array.shape[1]; j++)
      dx(i, j) = 0.5f * (array(i + 1, j) - array(i - 1, j));

  for (int j = 0; j < array.shape[1]; j++)
  {
    dx(0, j) = array(1, j) - array(0, j);
    dx(array.shape[0] - 1, j) =
        array(array.shape[0] - 1, j) - array(array.shape[0] - 2, j);
  }
  return dx;
}

Array gradient_y(const Array &array)
{
  Array dy = Array(array.shape);
  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 1; j < array.shape[1] - 1; j++)
      dy(i, j) = 0.5f * (array(i, j + 1) - array(i, j - 1));

  for (int i = 0; i < array.shape[0]; i++)
  {
    dy(i, 0) = array(i, 1) - array(i, 0);
    dy(i, array.shape[1] - 1) =
        array(i, array.shape[1] - 1) - array(i, array.shape[1] - 2);
  }
  return dy;
}

Array interp_nearest(const Array &x,
                     const Array &y,
                     const Array &z,
                     const Array &xi,
                     const Array &yi)
{
  std::vector<int> shape = x.shape;
  Array            zi = Array(shape);

  float xmin = x.min();
  float xmax = x.max();
  float ymin = y.min();
  float ymax = y.max();

  float ax = (shape[0] - 1) / (xmax - xmin);
  float ay = (shape[1] - 1) / (ymax - ymin);
  float bx = -xmin * (shape[0] - 1) / (xmax - xmin);
  float by = -ymin * (shape[1] - 1) / (ymax - ymin);

  for (int i = 0; i < shape[0]; i++)
    for (int j = 0; j < shape[1]; j++)
    {
      int p = (int)(ax * xi(i, j) + bx);
      int q = (int)(ay * yi(i, j) + by);

      p = std::max(0, p);
      q = std::max(0, q);
      p = std::min(shape[0] - 1, p);
      q = std::min(shape[1] - 1, q);

      zi(i, j) = z(p, q);
    }

  return zi;
}

std::vector<uint8_t> Array::to_img_8bit_grayscale()
{
  std::vector<uint8_t> data(this->shape[0] * this->shape[1]);
  const float          vmax = this->max();
  const float          vmin = this->min();

  if (vmax != vmin) // export a black image if not
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

std::vector<uint8_t> Array::to_img_8bit_rgb(Array *p_mask)
{
  std::vector<uint8_t> data(this->shape[0] * this->shape[1] * 3);
  const float          vmax = this->max();
  const float          vmin = this->min();

  std::vector<std::vector<float>> colors = {
      {0.001f, 0.000f, 0.014f},
      {0.070f, 0.050f, 0.194f},
      {0.198f, 0.064f, 0.404f},
      {0.348f, 0.083f, 0.494f},
      {0.494f, 0.141f, 0.508f},
      {0.639f, 0.190f, 0.494f},
      {0.786f, 0.242f, 0.450f},
      {0.913f, 0.330f, 0.383f},
      {0.980f, 0.491f, 0.368f},
      {0.996f, 0.661f, 0.451f},
      {0.995f, 0.827f, 0.586f},
      {0.987f, 0.991f, 0.750f},
  };
  int nc = (int)colors.size();

  if (vmax != vmin) // export a black image if not
  {
    float a = 1.f / (vmax - vmin);
    float b = -vmin / (vmax - vmin);
    int   k = 0;

    for (int j = this->shape[1] - 1; j > -1; j--)
      for (int i = 0; i < this->shape[0]; i++)
      {
        std::vector<float> rgb(3);
        float              v = a * (*this)(i, j) + b; // in [0, 1]
        float              vc = v * (float)(nc - 1);  // in [0, nc - 1]
        int                ic = (int)vc;

        if (ic == nc - 1)
          rgb = colors[nc - 1];
        else
        {
          float t = vc - (float)ic;
          for (int p = 0; p < 3; p++)
            rgb[p] = (1.f - t) * colors[ic][p] + t * colors[ic + 1][p];
        }

        if (p_mask)
          if ((*p_mask)(i, j) > 0.f)
            for (int p = 0; p < 3; p++)
              rgb[p] = 0.f;

        data[++k] = (uint8_t)std::floor(255 * rgb[1]); // G
        data[++k] = (uint8_t)std::floor(255 * rgb[2]); // B
        data[++k] = (uint8_t)std::floor(255 * rgb[0]); // R
      }
  }
  return data;
}

void Array::to_texture(GLuint &image_texture, int colormap, Array *p_mask)
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

  case 1:
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 this->shape[0],
                 this->shape[1],
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 this->to_img_8bit_rgb(p_mask).data());
    break;
  }
}
