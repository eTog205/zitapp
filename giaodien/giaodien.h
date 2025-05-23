// giaodien.h
#pragma once
#include <chrono>
#include <functional>
#include <imgui.h>

using demtg = std::chrono::steady_clock;

struct giaodien
{
	// --- Kích thước và vị trí menu bên ---
	float chieurong_menuben = 180.0f; // Chiều rộng hiện tại của menu bên
	float chieurong_menuben_thugon = 50.0f; // Chiều rộng khi thu gọn
	float chieurong_menuben_morong = 180.0f; // Chiều rộng khi mở rộng
	float letrai_menuben = 14.0f; // Lề trái của menu bên
	float letren_menuben = 125.0f; // Lề trên của menu bên
	float chieucao_menuben = 0.0f; // Chiều cao menu bên (tính theo chiều cao màn hình)

	// --- Trạng thái thu gọn/mở rộng menu ---
	bool menuben_thugon = false; // Trạng thái thu gọn: true nếu menu đang thu gọn
	bool yeucau_thugon = false; // Đánh dấu yêu cầu bắt đầu hiệu ứng thu gọn
	std::chrono::steady_clock::time_point batdau_thugon = std::chrono::steady_clock::now(); // Thời điểm bắt đầu hiệu ứng
	float thoigian_thugon = 0.3f; // Thời gian hiệu ứng thu gọn (giây)

	// --- Vị trí bảng dữ liệu ---
	float letren_bang = 125.0f; // Lề trên của bảng dữ liệu

	// --- Trạng thái menu phụ ---
	bool is_menu_open = false; // Trạng thái hiển thị menu nhỏ
};

extern giaodien gd;

struct thongtin_cuaso_imgui
{
	ImVec2 vitri;
	ImVec2 kichthuoc;
};

extern bool hien_cuaso_key;

thongtin_cuaso_imgui tinh_thongtin_cuaso(int chieurong_manhinh, int chieucao_manhinh);

void capnhat_bang_phanmem();

void giaodien_thanhcongcu(int chieurong_manhinh, int chieucao_manhinh);
void giaodien_menuben(int chieucao_manhinh);
void giaodien_caidat(thongtin_cuaso_imgui& tt);
void giaodien_bangdl(thongtin_cuaso_imgui& tt);
void giaodien_tienich(thongtin_cuaso_imgui& tt);
void giaodien_hotro(thongtin_cuaso_imgui& tt);

void hienthi_nhapkey();

void hienthi_loi(const std::string& loi, demtg::time_point tg_loi, int tg_tb_mat = 3);
void giaodien_tinhnang_xuatnap_cauhinh();
void giaodien_cuasotuychinh_menunho();

void hienthi_bangtienich();


struct nut_header
{
	const char* ten;
	std::function<void()> ham = nullptr;
	std::string tooltip;
};

struct nut_flex
{
	std::function<void()> ve;
	std::string tooltip;
};

struct TabItem
{
	std::string ten_hienthi;
	std::function<void(thongtin_cuaso_imgui&)> ham;
};

extern std::vector<TabItem> danhsach_tab;
extern int tab_index;
