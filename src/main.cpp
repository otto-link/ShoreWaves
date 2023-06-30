// Copyright (c) 2023 Otto Link. Distributed under the terms of the
// GNU General Public License. The full license is in the file
// LICENSE, distributed with this software.
#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/array.hpp"
#include "gui/utils.hpp"

int main()
{
  GLFWwindow *window = init_gui(1200, 800, "ShoreWaves (c) 2023 Otto Link");

  ImVec4 clear_color = ImVec4(0.15f, 0.25f, 0.30f, 1.00f);

  GLuint image_texture = 0;
  glGenTextures(1, &image_texture);

  Array z = Array({512, 512});
  z.randomize(0.f, 1.f, 1);

  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // --- processing
    
    z.to_texture(image_texture, 0);

        
    // --- GUI

    {
      ImGui::Begin("Settings");
      ImGui::Text("Blabla");
      ImGui::End();
    }

    {
      ImGui::Begin("Visualization");
    
      {
        ImVec2 win_size = ImGui::GetWindowSize();
        float  img_scaling =
            std::min(win_size[0] / z.shape[0], win_size[1] / z.shape[1]);
        ImVec2 img_size = {img_scaling * z.shape[0], img_scaling * z.shape[1]};
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
