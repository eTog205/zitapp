#pragma once
#include <optional>
#include <string>

inline auto macdinh_port = "443";
inline const std::string user_agent_macdinh = "ZitApp/1.0";

struct thamso_http
{
	std::string url; // dạng đầy đủ: https://host/target
	std::string method = "GET"; // "GET" hoặc "POST"
	std::string body; // dùng cho POST
	std::string user_agent = user_agent_macdinh;
	std::string accept = "*/*";
	bool is_asset = false;
	bool cho_phep_redirect = true;
	int so_lan_redirect = 0;
};

// 🔹 Cấu hình cho GitHub
struct cauhinh_github
{
	std::string host = "api.github.com";
	std::string port = macdinh_port;
	std::string accept_raw = "application/vnd.github.v3.raw";
	std::string accept_json = "application/json";
	std::string owner = "eTog205";
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
struct cauhinh_chung //sẽ bị xóa trong cập nhật tới
{
	std::uint64_t gioihan_body = 50ULL * 1024 * 1024;
};

// 🔹 Kết quả kiểm tra key
struct ketqua_key
{
	bool hop_le = false;
	std::string ngayhethan;
};

std::string gui_http_request(const thamso_http& ts);
std::string gui_post_api(const std::string& noidung_json);
std::string tai_raw_github_file(const std::string& target);
std::string tai_url_http(const std::string& url, bool is_asset = false, int redirect_count = 0);

std::string lay_thongtin_github(const std::string& file_path);
std::string lay_thongtin_tep_github(const std::string& file_path);
std::string get_release_tag();

std::optional<ketqua_key> kiemtra_key_online(const std::string& key);


