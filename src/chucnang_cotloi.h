// chucnang_cotloi.h
#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>

void chaylenh(const std::string& id);
void suachua_nhieu(const std::vector<std::string>& commands);
std::vector<std::string> tao_lenh_chkdsk();

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

void tienhanh_xoa();
