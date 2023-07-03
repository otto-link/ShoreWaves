// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#include <vector>

#include "FastNoiseLite.h"
#include "macrologger.h"

#include "core/array.hpp"

Array fbm_perlin(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 int                octaves,
                 float              weight,
                 float              persistence,
                 float              lacunarity,
                 std::vector<float> shift)
{
  Array         array = Array(shape);
  FastNoiseLite noise(seed);

  noise.SetFrequency(1.0f);
  noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
  noise.SetFractalOctaves(octaves);
  noise.SetFractalLacunarity(lacunarity);
  noise.SetFractalGain(persistence);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalWeightedStrength(weight);

  float ki = kw[0] / (float)shape[0];
  float kj = kw[1] / (float)shape[1];

  for (int i = 0; i < array.shape[0]; i++)
    for (int j = 0; j < array.shape[1]; j++)
      array(i, j) =
          noise.GetNoise(ki * (float)i + shift[0], kj * (float)j + shift[1]);

  return array;
}
