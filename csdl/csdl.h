// csdl.h
#pragma once
//#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <sqlite3.h>

namespace beast = boost::beast;
//namespace http = beast::http;
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

int open_database_read_only(const char* db_name);
void close_database();

int execute_sql(const char* sql);
int create_table();
int get_row_count(const char* table_name, int* row_count);

bool database_exists(const char* db_name);
void khoidong_sql();
