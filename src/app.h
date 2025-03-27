//app.h
#pragma once
#include <GLFW/glfw3.h>

struct cauhinh_cuaso
{
	int chieurong;
	int chieucao;
	const char* tieude;

	static cauhinh_cuaso macdinh()
	{
		return { 1024, 700, "Tông" };
	}
};

void dat_taskbar_icon(GLFWwindow* cuaso);
void glfw_error_callback(int loi, const char* mota);
GLFWwindow* khoitao_cuaso(const cauhinh_cuaso& ch_cuaso = cauhinh_cuaso::macdinh());
void vonglap_chinh(GLFWwindow* cuaso);
void dondep(GLFWwindow* cuaso);


