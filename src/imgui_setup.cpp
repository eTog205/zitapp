// imgui_setup.cpp
#include "imgui_setup.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <map>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <ranges>
#include <stb_image.h>

namespace
{
	void phongcach_win11()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		// ==== BỐ CỤC ====
		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.60f;
		style.WindowPadding = ImVec2(12, 12);
		style.WindowRounding = 8.0f;
		style.WindowBorderSize = 1.0f;
		style.ChildRounding = 6.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 6.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(10, 6);
		style.FrameRounding = 6.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = ImVec2(10, 8);
		style.ItemInnerSpacing = ImVec2(6, 6);
		style.CellPadding = ImVec2(6, 4);
		style.IndentSpacing = 20.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 6.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 6.0f;
		style.TabRounding = 6.0f;
		style.TabBorderSize = 1.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.5f);

		// ==== MÀU SẮC ====
		colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // đen
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // xám nhạt
		colors[ImGuiCol_WindowBg] = ImVec4(0.99f, 0.99f, 0.99f, 1.00f); // nền trắng xám
		colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.78f, 0.88f, 1.00f, 1.00f); // xanh nhạt hover
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.65f, 0.85f, 1.00f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.80f, 0.90f, 1.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.97f, 0.97f, 0.97f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.20f, 0.45f, 0.85f, 1.00f); // xanh Fluent
		colors[ImGuiCol_SliderGrab] = ImVec4(0.20f, 0.45f, 0.85f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.15f, 0.40f, 0.80f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.65f, 0.85f, 1.00f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.97f, 0.97f, 0.97f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.85f, 1.00f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.60f, 0.80f, 1.00f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.60f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.60f, 0.60f, 0.80f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.70f, 0.85f, 1.00f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.65f, 0.80f, 1.00f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.85f, 0.90f, 1.00f, 1.00f);

		// ==== MÀU CHO TABLE ====
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.97f, 0.97f, 0.97f, 1.00f); // tiêu đề bảng
		// colors[ImGuiCol_TableBorderStrong] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f); // viền chính
		// colors[ImGuiCol_TableBorderLight] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f); // viền phụ
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0, 0, 0, 0); // tắt viền chính
		colors[ImGuiCol_TableBorderLight] = ImVec4(0, 0, 0, 0); // tắt viền phụ
		colors[ImGuiCol_TableRowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // dòng nền chính
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f); // dòng xen kẽ
	}

	std::map<std::string, TextureInfo> icon_cache;

	void giaiphong_texture_icon()
	{
		for (const auto& info : icon_cache | std::views::values)
			glDeleteTextures(1, &info.texture_id);
		
		icon_cache.clear();
	}

} // namespace

void caidat_font()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("tainguyen/arial.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesVietnamese());
	// io.MouseDragThreshold = 10.0f;
}

TextureInfo tai_texture_tu_tep(const char* duongdan)
{
	std::string path(duongdan);
	if (icon_cache.contains(path))
		return icon_cache[path];

	int width, height, channels;
	unsigned char* data = stbi_load(duongdan, &width, &height, &channels, 4);
	if (!data)
		return { .texture_id = 0, .width = 0, .height = 0 };

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	TextureInfo info = { .texture_id = texture, .width = width, .height = height };
	icon_cache[path] = info;
	return info;
}

void khoitao_imgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	caidat_font();
	phongcach_win11();
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
	giaiphong_texture_icon();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
