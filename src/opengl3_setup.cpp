//opengl3_setup.cpp
#include "opengl3_setup.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_opengl3.h>

void khoitao_opengl()
{
	ImGui_ImplOpenGL3_Init("#version 130");
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void ve_khunghinh(const int chieurong, const int chieucao)
{
	ImGui::Render();
	glViewport(0, 0, chieurong, chieucao);
	glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFinish();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void dondep_opengl()
{
	ImGui_ImplOpenGL3_Shutdown();
}


