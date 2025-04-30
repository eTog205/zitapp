// giaodien.h
#pragma once
#include <chrono>
#include <imgui.h>

using demtg = std::chrono::steady_clock;

struct giaodien
{
	float botron_nen = 10.0f; // Bán kính bo tròn góc của các khung hiển thị
	float botron_nho = 4.0f; // Bán kính bo tròn góc của thành phần nhỏ(nút, combo box, ô)

	float chieucao_thanhtieude = 23.0f;

	// Kích thước và vị trí của menu bên
	float chieurong_menuben = 180.0f; // Chiều rộng ban đầu của menu bên
	float chieurong_menuben_thugon = 50.0f; // Chiều rộng menu bên khi thu gọn
	float chieurong_menuben_morong = 180.0f; // Chiều rộng tối đa khi mở rộng menu
	float letrai_menuben = 14.0f; // Khoảng cách từ lề trái của màn hình đến menu bên
	float letren_menuben = 125.0f; // Khoảng cách từ lề trên của màn hình đến menu bên
	float chieucao_menuben = 0.0f; // Chiều cao menu bên (tính theo chiều cao màn hình)

	// Trạng thái thu gọn/mở rộng của menu bên
	bool menuben_thugon = false; // Biến trạng thái: true nếu menu đang thu gọn
	bool yeucau_thugon = false; // Biến yêu cầu thu gọn/mở rộng menu
	std::chrono::steady_clock::time_point batdau_thugon = std::chrono::steady_clock::now(); // Thời điểm bắt đầu hiệu ứng thu gọn/mở rộng
	float thoigian_thugon = 0.3f; // Thời gian hiệu ứng thu gọn/mở rộng menu (giây)

	// Kích thước và vị trí của bảng dữ liệu
	float chieurong_bang = 0.0f;
	float letrai_bang = 14.0f;
	float letren_bang = 125.0f;
	float chieucao_bang = 0.0f; // Chiều cao của bảng dữ liệu (tính theo chiều cao màn hình)

	bool isMenuOpen = false;
};

extern giaodien gd;

struct MenuItem
{
	std::wstring toanbo_vanban;
	std::string id;
};

struct thongtin_cuaso_imgui
{
	ImVec2 vitri;
	ImVec2 kichthuoc;
};

extern bool hien_cuaso_key;

thongtin_cuaso_imgui tinh_thongtin_cuaso(int chieurong_manhinh, int chieucao_manhinh);

ImVec4 adjust_color_brightness(const ImVec4& color, float factor);

void capnhat_bang_phanmem();

void giaodien_thanhcongcu(int chieurong_manhinh, int chieucao_manhinh);
void giaodien_menuben(int chieucao_manhinh);
void giaodien_caidat(int chieurong_manhinh, int chieucao_manhinh);
void giaodien_bangdl(int chieurong_manhinh, int chieucao_manhinh);
void giaodien_tienich(int chieurong_manhinh, int chieucao_manhinh);
void giaodien_hotro(const int chieurong_manhinh, const int chieucao_manhinh);

void hienthi_nhapkey();

void hienthi_loi(const std::string& loi, demtg::time_point tg_loi, int tg_tb_mat = 3);
void giaodien_tinhnang_xuatnap_cauhinh();
void giaodien_cuasotuychinh_menunho();

extern std::string tab_hientai;
void hienthi_bangsuachua();
