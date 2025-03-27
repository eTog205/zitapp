// imgui_setup.cpp
#include "imgui_setup.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "imgui.h"

void caidat_font()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("tainguyen/arial.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesVietnamese());
	//io.MouseDragThreshold = 10.0f;
}

void khoitao_imgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 10.0f;
	style.FrameRounding = 4.0f;

	caidat_font();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
}

void ve_imgui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void dondep_imgui()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


