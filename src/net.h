#pragma once
#include <optional>
#include <string>

// 🔹 Cấu hình cho GitHub
struct cauhinh_github
{
	std::string host = "api.github.com";
	std::string port = "443";
	std::string accept_raw = "application/vnd.github.v3.raw";
	std::string accept_json = "application/json";
	std::string owner;
	std::string repo;
};

// 🔹 Cấu hình yêu cầu POST JSON
struct cauhinh_api
{
	std::string host = "api.tntstudio.io.vn";
	std::string port = "443";
	std::string target = "/";
};

// 🔹 Cấu hình mặc định
struct cauhinh_chung
{
	std::string host;
	std::string port = "443";
	std::string target;
	std::string accept = "application/json";
	std::uint64_t gioihan_body = 50 * 1024 * 1024;
};

// 🔹 Kết quả kiểm tra key
struct ketqua_key
{
	bool hop_le = false;
	std::string ngayhethan;
};

// 🔸 Gửi GET request
std::string http_get(const cauhinh_chung& ch);

// 🔸 Gửi POST request
std::string http_post(const cauhinh_chung& ch, const std::string& noidung_json);

// 🔸 Tải nội dung file GitHub
std::string tai_file_github(const cauhinh_github& ch, const std::string& filepath);

// 🔸 Lấy SHA của file GitHub
std::string lay_sha_github(const cauhinh_github& ch, const std::string& filepath);

// 🔸 Gọi API kiểm tra key
std::optional<ketqua_key> kiemtra_key_online(const cauhinh_api& ap, const std::string& key);
