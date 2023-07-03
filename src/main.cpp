// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/array.hpp"
#include "core/fbm.hpp"
#include "core/gerstner.hpp"
#include "gui/gui.hpp"
#include "gui/utils.hpp"

int main()
{
  GLFWwindow *window = init_gui(1200, 800, "ShoreWaves (c) 2023 Otto Link");

  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  GLuint image_texture = 0;
  glGenTextures(1, &image_texture);

  // --- simulation default parameters

  std::vector<int> shape = {512, 512};

  WaterDepth    depth = WaterDepth(shape);
  GuiWaterDepth depth_gui = GuiWaterDepth(depth);

  GerstnerWave    wave = GerstnerWave(depth.h);
  GuiGerstnerWave wave_gui = GuiGerstnerWave(wave);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // --- processing

    // --- GUI

    {
      ImGui::Begin("Settings");
      ImGui::Text("Depth");

      ImGui::SeparatorText("Water depth");
      depth_gui.render();

      ImGui::SeparatorText("Ocean waves");

      if (depth_gui.updated)
      {
        depth_gui.updated = false;
        wave.update();
      }

      wave_gui.render();

      ImGui::SeparatorText("Fields");

      static int e = 3;
      ImGui::RadioButton("depth", &e, 0);
      ImGui::RadioButton("shore_dist", &e, 1);
      ImGui::RadioButton("phi_depth", &e, 2);
      ImGui::RadioButton("dz", &e, 3);

      static float t = 0.f;

      switch (e)
      {
      case 0:
        depth.h.to_texture(image_texture, 0);
        break;
      case 1:
        wave.shore_dist.to_texture(image_texture, 0);
        break;
      case 2:
        wave.phi_depth.to_texture(image_texture, 0);
        break;
      case 3:
        t += wave.kinf / 300.f;
        wave.generate(t);
        wave.dz.to_texture(image_texture, 1, &depth.h);
        break;
      }

      ImGui::End();
    }

    {
      ImGui::Begin("Visualization");

      {
        ImVec2 win_size = ImGui::GetWindowSize();
        float  img_scaling = std::min(win_size[0] / depth.h.shape[0],
                                     win_size[1] / depth.h.shape[1]);
        ImVec2 img_size = {img_scaling * depth.h.shape[0],
                           img_scaling * depth.h.shape[1]};
        ImGui::Image((void *)(intptr_t)image_texture, img_size);
      }

      ImGui::End();
    }

    // --- Rendering

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w,
                 clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w,
                 clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // --- Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
