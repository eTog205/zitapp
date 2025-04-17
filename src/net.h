#pragma once
#include <optional>
#include <string>

constexpr const char* macdinh_port = "443";

// 🔹 Cấu hình cho GitHub
struct cauhinh_github
{
	std::string host = "api.github.com";
	std::string port = macdinh_port;
	std::string accept_raw = "application/vnd.github.v3.raw";
	std::string accept_json = "application/json";
	std::string owner = "eTog205";
	//std::string repo;
	std::string repo_tainguyen = "tainguyen";
	std::string repo_app = "zitapp";
};

// 🔹 Cấu hình yêu cầu POST JSON
struct cauhinh_api
{
	std::string host = "api.tntstudio.io.vn";
	std::string port = macdinh_port;
	std::string accept = "application/json";
	std::string target = "/";
};

// 🔹 Cấu hình mặc định
struct cauhinh_chung
{
	//std::string host;
	std::string port = macdinh_port;
	//std::string target;
	std::string accept = "application/json";
	std::uint64_t gioihan_body = 50ULL * 1024 * 1024;
};

// 🔹 Kết quả kiểm tra key
struct ketqua_key
{
	bool hop_le = false;
	std::string ngayhethan;
};

std::string lay_thongtin_github(const std::string& file_path);
std::string lay_thongtin_tep_github(const std::string& file_path);
std::string get_release_tag();

// 🔸 Gửi POST request
std::string http_post(const std::string& noidung_json);

std::string gui_yeucau_http(const std::string& target);
std::string gui_yeucau_http2(const std::string& url, bool is_asset = false, int redirect_count = 0);

// 🔸 Gọi API kiểm tra key
std::optional<ketqua_key> kiemtra_key_online(const std::string& key);
