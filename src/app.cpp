// app.cpp
#include "app.h"

#include "chay_luongphu.h"
#include "csdl.h"
#include "dv_csdl.h"
#include "imgui_setup.h"
#include "log_nhalam.h"
#include "opengl3_setup.h"
#include "resource.h"

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>

void dat_taskbar_icon(GLFWwindow* cuaso)
{
	const HWND hwnd = glfwGetWin32Window(cuaso);

	if (auto h_icon = static_cast<HICON>(LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR)))
	{
		SetClassLongPtr(hwnd, GCLP_HICON, reinterpret_cast<LONG_PTR>(h_icon));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(h_icon));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL2, reinterpret_cast<LPARAM>(h_icon));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(h_icon));
	}
}
#endif

void glfw_error_callback(const int loi, const char* mota)
{
	td_log(loai_log::loi, "GLFW lỗi %d: %s\n" + std::to_string(loi) + std::string(mota));
}

GLFWwindow* khoitao_cuaso(const cauhinh_cuaso& ch_cuaso)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		td_log(loai_log::loi, "khởi tạo glfw thất bại");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(ch_cuaso.chieurong, ch_cuaso.chieucao, ch_cuaso.tieude, nullptr, nullptr);
	if (!window)
	{
		td_log(loai_log::loi, "khởi tạo cửa sổ glfw thất bại");
		glfwTerminate();
		return nullptr;
	}

	glfwSetWindowSizeLimits(window, ch_cuaso.chieurong, ch_cuaso.chieucao, GLFW_DONT_CARE, GLFW_DONT_CARE);

	GLFWmonitor* manhinh_chinh = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(manhinh_chinh);

	const int vitri_x = (mode->width - ch_cuaso.chieurong) / 2;
	const int vitri_y = (mode->height - ch_cuaso.chieucao) / 2;
	glfwSetWindowPos(window, vitri_x, vitri_y);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	khoitao_imgui(window);
	khoitao_opengl();

	return window;
}

void vonglap_chinh(GLFWwindow* cuaso)
{
	// hiện tại để chờ, tương lai cải thiện
	auto f = lq_khoidong_sql();
	f.wait();

	while (!glfwWindowShouldClose(cuaso))
	{
		glfwPollEvents();

		if (glfwGetWindowAttrib(cuaso, GLFW_ICONIFIED))
		{
			glfwSetWindowShouldClose(cuaso, GLFW_FALSE);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		ve_imgui();

		int chieurong, chieucao;
		glfwGetFramebufferSize(cuaso, &chieurong, &chieucao);

		ve_giaodien(chieurong, chieucao);

		ve_khunghinh(chieurong, chieucao);
		glfwSwapBuffers(cuaso);
	}
}

void dondep(GLFWwindow* cuaso)
{
	close_database();
	dondep_opengl();
	dondep_imgui();

	glfwDestroyWindow(cuaso);
	glfwTerminate();
}
