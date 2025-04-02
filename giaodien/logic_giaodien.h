// logic_giaodien.h
#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <filesystem>

namespace fs = std::filesystem;

struct column_config
{
	std::string id;
	std::string tieude;
	float chieurong_hientai;
	float chieurong_toithieu;
	float chieurong_toida;
	bool thaydoi_kt;
	bool hienthi;
	bool sapxep;

	column_config(std::string id, std::string tieude, const float chieurong = 100.0f, const float min_width = 40.0f, const float chieurong_toida = 1000.0f, const bool thaydoi_kt = true,
				  const bool hienthi = true, const bool sapxep = true)
		: id(std::move(id)), tieude(std::move(tieude)), chieurong_hientai(chieurong), chieurong_toithieu(min_width), chieurong_toida(chieurong_toida), thaydoi_kt(thaydoi_kt), hienthi(hienthi),
		  sapxep(sapxep)
	{}
};

struct table_config
{
	std::vector<column_config> columns;

	void add_column(const column_config& col)
	{
		columns.push_back(col);
	}

	void reorder_columns(const std::vector<std::string>& new_order)
	{
		std::vector<column_config> new_cols;
		for (const auto& id : new_order)
		{
			for (const auto& cot : columns)
			{
				if (cot.id == id)
				{
					new_cols.push_back(cot);
					break;
				}
			}
		}

		for (const auto& cot : columns)
		{
			bool found = false;
			for (const auto& id : new_order)
			{
				if (cot.id == id)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				new_cols.push_back(cot);
			}
		}
		columns = std::move(new_cols);
	}
};

struct logic_giaodien
{
	table_config ch_b;
	std::vector<std::vector<std::string>> data;
	std::unordered_map<std::string, bool> selected_map;

	int dem_hang;

	void khoidong_bang_dl();

	static void chaylenh_winget(const std::string& lenh_id);
	static void chaylenh_tienich();
};

extern logic_giaodien lg_gd;

std::string wstring_to_string(const std::wstring& wch);
std::wstring string_to_wstring(const std::string& chuoi);

float tt_thugonkichthuoc(bool& da_thugon, bool& yeucau_thugon, const std::chrono::steady_clock::time_point& thoigian_batdau_thugon, float kichthuoc_morong, float kichthuoc_thugon,
						 float thoigian_tre);

std::string tt_vanbancothenhinthay(const std::wstring& toanbo_vanban, float chieurong_hientai, float chieurong_toithieu, float chieurong_toida);

struct dulieuduongdan
{
	std::string dd_xuat;
	std::string dd_nap;
	std::string loi_xuat_tepch;
	std::string loi_nap_tepch;
	std::chrono::steady_clock::time_point thoigian_hienthi_loi_xuat;
	std::chrono::steady_clock::time_point thoigian_hienthi_loi_nap;
};

extern dulieuduongdan dl;

struct cauhinh
{
	fs::path tep_nguon;
	fs::path thumuc_ch = "tainguyen";
	fs::path tep_dich = thumuc_ch / "cauhinh.ini";
};

extern cauhinh ch;

void tao_thumuc_tainguyen();

void saochep_ini();

bool kiemtra_duongdan(const std::string& duongdan_str, bool (*ham_kiemtra)(const fs::path&), const char* thongbao_loi, bool chophep_rong, std::string& loi);
bool kiemtraduongdan_thumuc();
bool kiemtraduongdan_taptin();

void xuat_cauhinh(const std::string& duongdan_xuat);
void nap_cauhinh();

template <typename T>
void capnhat_cauhinh(const boost::property_tree::ptree& pt, const std::string& key, T& var)
{
	if (auto opt = pt.get_optional<T>(key))
	{
		var = *opt;
	}
}

void ch_macdinh();

bool kiemtra_khoapro(const std::string& key, std::string& ten_nguoidung, std::string& loi);
