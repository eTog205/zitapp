// chucnang_cotloi.h
#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

enum class chedo : std::uint8_t
{
	imlang,
	theodoi
};

enum class nhom : std::uint8_t
{
	khuyennghi,
	tuychon
};

struct cauhinh_xoa
{
	std::unordered_map<nhom, bool> batNhom;
	bool imlang;
	cauhinh_xoa(bool imlang_macdinh = true) : batNhom{ { nhom::khuyennghi, true }, { nhom::tuychon, false } }, imlang(imlang_macdinh)
	{}
};

struct mucdich_xoa
{
	fs::path duongdan;
	nhom nhom;
};

struct ThongKe
{
	uintmax_t daTimThay = 0;
	uintmax_t daXoa = 0;
	uintmax_t tongDungLuongDaXoa = 0;
	uintmax_t biBoQua = 0;
};

// chức năng tiện ích
void chaylenh(const std::string& id);
void suachua_nhieu(const std::vector<std::string>& commands);
std::vector<std::string> tao_lenh_chkdsk();
void tienhanh_xoa();
bool mo_phanmanh();
bool chay_phanmanh();
