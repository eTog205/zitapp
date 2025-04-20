//capnhat.h
#pragma once
#include <string>

struct thongtin
{
	std::string phienban_sosanh;
	std::string tentep_phienban = "phienban.txt";
	std::string tentep_appchinh = "zitapp.exe";
};

std::string get_appcoban_path();

std::string doctep_phienban(const std::string& tep_phienban);
void ghitep_phienban(const std::string& tep_phienban, const std::string& dl_canghi);

int loc_dl(const std::string& dauvao);
std::string dinhdang_dl(int so_phienban);

bool sosanh_phienban(int so_hientai, int so_layve);

void kiemtra_capnhat();
void capnhat();
void chay_app_co_ban();


