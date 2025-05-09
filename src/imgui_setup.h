// imgui_setup.h
#pragma once
#include <GLFW/glfw3.h>

void caidat_font();
void khoitao_imgui(GLFWwindow* window);
void ve_imgui();
void dondep_imgui();

struct TextureInfo
{
	GLuint texture_id;
	int width;
	int height;
};

TextureInfo tai_texture_tu_tep(const char* duongdan);
