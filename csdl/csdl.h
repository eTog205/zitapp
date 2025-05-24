// csdl.h
#pragma once
#include <boost/beast/ssl.hpp>
#include <sqlite3.h>

namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = net::ip::tcp;

struct csdl
{
	std::string thumuc = "tainguyen/";
	std::string file_path = "sql.db";
	std::string sha_file = "sql.sha";
};

void save_to_file(const std::string& filename, const std::string& data);

void luu_tepsha(const std::string& thumuc, const std::string& sha_file, const std::string& file_path);

void capnhat_data(const csdl& c);

extern sqlite3* db;

int open_database_read_only(const char* ten_db);
void close_database();

int execute_sql(const char* sql);
int create_table();
int get_row_count(std::string_view ten_bang, int* row_count);

void khoidong_sql();
