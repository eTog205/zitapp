#pragma once
#include <functional>
#include <imgui.h>
#include <string>
#include <unordered_map>

struct hieuung_cuaso
{
	bool hien = false;
	bool dangchay = false;
	float thoigian = 0.0f;

	ImVec2 tu_vitri;
	ImVec2 den_vitri;
	ImVec2 vitri_hientai;
	float do_mo = 1.0f;

	std::function<void(hieuung_cuaso&, float)> capnhat;
};

class ql_hieuung
{
public:
	void capnhat(float dt);
	void an(const std::string& ten_cuaso);
	void chay_hieuung_truot(const std::string& ten_cuaso, ImVec2 tu, ImVec2 den, float tocdo = 5.0f);

	bool dangmo(const std::string& ten_cuaso);

	const hieuung_cuaso& lay(const std::string& ten_cuaso) const;

private:
	std::unordered_map<std::string, hieuung_cuaso> danhsach;
};

bool hieu_ung_mo_len(float thoi_gian = 0.5f);
bool hieu_ung_mo_len_dungten(const char* ten_cuaso, float thoigian = 0.5f);
