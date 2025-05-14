// dv_csdl.cpp
#include "dv_csdl.h"
#include "csdl.h"
#include "giaodien.h"
#include "log_nhalam.h"
#include "logic_giaodien.h"

void nap_du_lieu()
{
	int row_count = 0;
	if (get_row_count("Items", &row_count) == SQLITE_OK && row_count > 0)
	{
		lg_gd.dem_hang = row_count;
	}

	lg_gd.data.clear();

	const std::string sql = "SELECT ID, Name, Category FROM Items;";
	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			std::vector<std::string> row;
			row.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))); // ID
			row.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))); // Tên
			row.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))); // Phân loại
			lg_gd.data.push_back(row);
		}
	}
	else
	{
		td_log(loai_log::loi, "lấy dữ liệu từ Items!");
	}

	sqlite3_finalize(stmt);
	lg_gd.khoidong_bang_dl();
}

void ve_giaodien(const int chieurong_manhinh, const int chieucao_manhinh)
{
	thongtin_cuaso_imgui tt = tinh_thongtin_cuaso(chieurong_manhinh, chieucao_manhinh);

	giaodien_thanhcongcu(chieurong_manhinh, chieucao_manhinh);
	giaodien_menuben(chieucao_manhinh);
	giaodien_tienich(tt);
	giaodien_caidat(tt);
	giaodien_hotro(tt);
	giaodien_bangdl(tt);

	// ImGui::ShowDemoWindow();
}
