//csdl.h
#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <sqlite3.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

struct csdl
{
	std::string owner = "eTog205";
	std::string repo = "ZAppCoBan";
	std::string file_path = "sql.db";
	std::string sha_file = "sql.sha";
};

// 🔹 Hàm giải mã Base64 nếu cần
std::string decode_base64(const std::string& encoded);

// 🔹 Hàm gửi request HTTP GET
std::string send_http_request(const std::string& host, const std::string& target);

// 🔹 Hàm lấy nội dung `data.json` từ GitHub API
std::string fetch_github_data(const std::string& owner, const std::string& repo, const std::string& file_path);

std::string fetch_github_file_metadata(const std::string& owner, const std::string& repo, const std::string& file_path);

// 🔹 Hàm lưu nội dung ra file
void save_to_file(const std::string& filename, const std::string& data);

// 🔹 Hàm lưu sha ra file
void luu_tepsha(const std::string& sha_file, const std::string& owner, const std::string& repo, const std::string& file_path);

//void capnhat_data();
void capnhat_data(const csdl& c);

extern sqlite3* db;

int open_database_read_only(const char* db_name);
void close_database();

int execute_sql(const char* sql);
int create_table();
int get_row_count(const char* table_name, int* row_count);

bool database_exists(const char* db_name);
void khoidong_sql();


