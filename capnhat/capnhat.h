//capnhat.h
#pragma once
#include <string>

struct capnhat
{
	std::string phienban_sosanh;
};

std::string get_appcoban_path();

std::string doctep_phienban(const std::string& tentep);
void ghitep_phienban(const std::string& tentep, const std::string& dl_canghi);

int loc_dl(const std::string& dauvao);
std::string dinhdang_dl(int so_phienban);

bool sosanh_phienban(int so_hientai, int so_layve);

void kiemtra_capnhat();
void capnhat();
void chay_app_co_ban();


