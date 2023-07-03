// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#pragma once
#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "core/gerstner.hpp"

class GuiWaterDepth
{
public:
  WaterDepth &wd;
  bool        updated = false;

  GuiWaterDepth(WaterDepth &wd) : wd(wd)
  {
    this->width = this->wd.shape[0];
    this->height = this->wd.shape[1];
    this->seed = this->wd.seed;

    this->wd.update();
  }

  void render()
  {
    ImGui::Text("Domain size");

    if (ImGui::SliderInt("Width", &this->width, 32, 2048))
    {
      this->width -= this->width % 32;
      this->wd.set_shape({this->width, this->height});
      this->update();
    }

    if (ImGui::SliderInt("Height", &this->height, 32, 2048))
    {
      this->height -= this->height % 32;
      this->wd.set_shape({this->width, this->height});
      this->update();
    }

    ImGui::Text("fBm noise");

    if (ImGui::SliderFloat("Wavenumber x", &this->wd.kw[0], 0.1f, 64.f))
      this->update();

    if (ImGui::SliderFloat("Wavenumber y", &this->wd.kw[1], 0.1f, 64.f))
      this->update();

    if (ImGui::DragInt("Seed", &this->seed))
    {
      this->wd.seed = (uint)this->seed;
      this->update();
    }

    if (ImGui::InputInt("Octaves", &this->wd.octaves))
      this->update();

    if (ImGui::SliderFloat("Weight", &this->wd.weight, 0.f, 1.f))
      this->update();

    ImGui::Text("Sea floor");

    if (ImGui::SliderFloat("Slope", &this->wd.slope, 0.f, 4.f))
      this->update();

    if (ImGui::SliderFloat("Offset", &this->wd.offset, -1.f, 1.f))
      this->update();

    if (ImGui::SliderFloat("Scaling", &this->wd.scaling, 0.f, 10.f))
      this->update();
  }

  void update()
  {
    wd.update();
    this->updated = true;
  }

private:
  int width;
  int height;
  int seed;
};

class GuiGerstnerWave
{
public:
  GerstnerWave &w;
  bool          updated = false;

  GuiGerstnerWave(GerstnerWave &w) : w(w)
  {
    this->w.update();
  }

  void render()
  {
    if (ImGui::SliderFloat("Wavenumber", &this->w.kinf, 0.1f, 32.f))
      this->update();

    if (ImGui::SliderAngle("Wave angle", &this->w.alpha, -90.f, 90.f))
      this->update();

    if (ImGui::SliderFloat("Steepness", &this->w.steepness, 0.f, 1.f))
      this->update();

    if (ImGui::SliderFloat("Kludgeing", &this->w.kludge, 0.f, 100.f))
      this->update();

    if (ImGui::SliderFloat("Wavenumber clipping ratio",
                           &this->w.k_clipping_ratio,
                           0.f,
                           10.f))
      this->update();

    if (ImGui::SliderFloat("Shore / deep sea amplitude ratio",
                           &this->w.shore_r_ratio,
                           0.f,
                           1.f))
      this->update();

    if (ImGui::SliderFloat("Shore distance influence",
                           &this->w.shore_dist_ratio,
                           0.f,
                           10.f))
      this->update();
  }

  void update()
  {
    w.update();
    this->updated = true;
  }
};
