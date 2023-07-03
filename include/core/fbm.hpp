// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#pragma once

#include "core/array.hpp"

Array fbm_perlin(std::vector<int>   shape,
                 std::vector<float> kw,
                 uint               seed,
                 int                octaves,
                 float              weight,
                 float              persistence,
                 float              lacunarity,
                 std::vector<float> shift = {0.f, 0.f});
