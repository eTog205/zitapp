// csdl.cpp
#include "csdl.h"
#include "dv_csdl.h"
#include "log_nhalam.h"
#include "net.h"

#include <boost/beast/core.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/json.hpp>
#include <fstream>

namespace bj = boost::json;
sqlite3* db = nullptr;

// !!!loại bỏ trong cập nhật sau
std::string decode_base64(const std::string& encoded)
{
	std::string decoded;
	decoded.resize(beast::detail::base64::decoded_size(encoded.size()));
	auto len = beast::detail::base64::decode(decoded.data(), encoded.data(), encoded.size());
	decoded.resize(len.first);
	return decoded;
}

void save_to_file(const std::string& filename, const std::string& data)
{
	std::ofstream outFile(filename, std::ios::binary);
	if (outFile)
	{
		outFile.write(data.data(), static_cast<std::streamsize>(data.size()));
		outFile.close();
	}
	else
		td_log(loai_log::loi, "ghi file:" + std::string(filename));
}

void luu_tepsha(const std::string& thumuc, const std::string& sha_file, const std::string& file_path)
{
	std::ofstream sha_file_out(thumuc + sha_file);

	if (!sha_file_out)
	{
		td_log(loai_log::loi, "Không thể mở file SHA: " + sha_file);
		return;
	}

	std::string metadata_response = lay_thongtin_tep_github(file_path);

	if (metadata_response.empty())
	{
		td_log(loai_log::loi, "Không nhận được metadata từ GitHub");
		return;
	}

	try
	{
		bj::value metadata_json = bj::parse(metadata_response);

		if (!metadata_json.is_array())
		{
			td_log(loai_log::loi, "Metadata không phải mảng JSON");
			return;
		}
		auto& arr = metadata_json.as_array();
		if (arr.empty() || !arr[0].is_object())
		{
			td_log(loai_log::loi, "Mảng metadata rỗng hoặc phần tử đầu tiên không phải object");
			return;
		}

		auto& obj0 = arr[0].as_object();
		auto it = obj0.find("sha");
		if (it == obj0.end())
		{
			td_log(loai_log::loi, "Metadata không chứa `sha`");
			return;
		}

		auto new_sha = bj::value_to<std::string>(it->value());
		sha_file_out << new_sha; // Ghi ra file
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "Lỗi parse JSON: " + std::string(e.what()));
	}

	sha_file_out.close();
}

void capnhat_data(const csdl& c)
{
	// Đọc SHA cũ
	std::ifstream sha_file_in(c.thumuc + c.sha_file);
	std::string old_sha;
	if (sha_file_in)
	{
		std::getline(sha_file_in, old_sha);
		sha_file_in.close();
	}

	// Nếu có SHA cũ, so sánh với SHA mới
	if (!old_sha.empty())
	{
		std::string metadata_response = lay_thongtin_tep_github(c.file_path);
		if (metadata_response.empty())
		{
			td_log(loai_log::loi, "Không thể lấy metadata từ GitHub");
			return;
		}

		bj::value metadata_json;
		try
		{
			metadata_json = bj::parse(metadata_response);
		} catch (const std::exception& e)
		{
			td_log(loai_log::loi, "Lỗi parse metadata: " + std::string(e.what()));
			return;
		}

		if (!metadata_json.is_array())
		{
			td_log(loai_log::loi, "Metadata không phải mảng JSON");
			return;
		}
		auto& arr = metadata_json.as_array();
		if (arr.empty() || !arr[0].is_object())
		{
			td_log(loai_log::loi, "Mảng metadata rỗng hoặc phần tử đầu tiên không phải object");
			return;
		}
		auto& obj0 = arr[0].as_object();
		auto it = obj0.find("sha");
		if (it == obj0.end())
		{
			td_log(loai_log::loi, "Metadata không chứa `sha`");
			return;
		}

		auto new_sha = bj::value_to<std::string>(it->value());
		// Nếu SHA không thay đổi, dừng
		if (old_sha == new_sha)
			return;
	}

	// Nếu SHA cũ không tồn tại hoặc đã thay đổi => tải file mới
	td_log(loai_log::thong_bao, "🔄 Dữ liệu mới có phiên bản cập nhật, tiến hành tải...");

	std::string new_data = lay_thongtin_github(c.file_path);
	if (!new_data.empty())
	{
		save_to_file(c.thumuc + c.file_path, new_data);
		luu_tepsha(c.thumuc, c.sha_file,   c.file_path);

		td_log(loai_log::thong_bao, "Đã cập nhật dữ liệu và lưu SHA mới.");
	}
	else
	{
		td_log(loai_log::loi, "Không tải được dữ liệu `" + c.file_path + "`");
	}
}

int open_database_read_only(const char* db_name)
{
	return sqlite3_open_v2(db_name, &db, SQLITE_OPEN_READONLY, nullptr);
}

void close_database()
{
	if (db)
	{
		sqlite3_close(db);
		db = nullptr;
	}
}

int get_row_count(const char* table_name, int* row_count)
{
	*row_count = 0;
	const std::string sql = "SELECT COUNT(*) FROM " + std::string(table_name) + ";";
	sqlite3_stmt* stmt;

	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		td_log(loai_log::loi, "Failed to prepare statement" + std::string(sqlite3_errmsg(db)));
		return rc;
	}

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		*row_count = sqlite3_column_int(stmt, 0);
	}
	else
	{
		td_log(loai_log::loi, "Failed to execute statement:" + std::string(sqlite3_errmsg(db)));
	}

	sqlite3_finalize(stmt);
	return (rc == SQLITE_ROW) ? SQLITE_OK : rc;
}

int execute_sql(const char* sql)
{
	char* err_msg = nullptr;
	const int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);

	if (rc != SQLITE_OK)
	{
		td_log(loai_log::loi, "SQL" + std::string(err_msg));
		sqlite3_free(err_msg);
	}

	return rc;
}

int create_table()
{
	const auto sql = "CREATE TABLE IF NOT EXISTS Items ("
					 "ID TEXT PRIMARY KEY, "
					 "Name TEXT NOT NULL, "
					 "Category TEXT);";

	return execute_sql(sql);
}

bool database_exists(const char* db_name)
{
	return std::filesystem::exists(db_name);
}

void khoidong_sql()
{
	open_database_read_only("tainguyen/sql.db");
	nap_du_lieu();
}
