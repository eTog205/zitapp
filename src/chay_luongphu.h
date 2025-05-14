// chay_luongphu.h
#pragma once
#include "net.h"

#include <future>
#include <optional>
#include <string>

std::future<void> lp_chay_capnhat();
std::future<void> lq_khoidong_sql();

void lp_chay_lenhwinget(const std::string& id);
void lp_suachua_nhieu(bool dism, bool sfc, bool chk);
void lp_mo_phanmanh();
void lp_chay_phanmanh();
void lp_chay_xoarac();

void lp_nap_cauhinh();
void lp_xuat_cauhinh(const std::string& dd);
void lp_nap_cauhinh_macdinh();
void lp_chay_saochep_ini();
void lq_kiemtra_key(const std::string& key_nhap);

std::future<std::optional<ketqua_key>> lq_kiemtra_key_async(const std::string& key);
