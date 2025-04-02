#pragma once
#include <boost/unordered_map.hpp>
#include <cstdint>
#include <optional>


enum thongbao_giaodien :uint8_t
{
	sha256_mismatch,
	abc
};

// Cấu trúc chứa dữ liệu cho từng ID
struct Data
{
	std::optional<int> intValue;
	std::optional<std::string> stringValue;
	std::optional<bool> boolValue;
};

// Lớp quản lý dữ liệu
class DataManager
{
public:
	boost::unordered_map<int, Data> storage;

	// Thêm ID mới nếu chưa tồn tại
	void addID(int id);

	// Gán giá trị int cho ID
	void setInt(int id, int value);

	// Gán giá trị string cho ID
	void setString(int id, const std::string& value);

	// Gán giá trị bool cho ID
	void setBool(int id, bool value);

	// Lấy giá trị int từ ID
	std::optional<int> getInt(int id);
	// Lấy giá trị string từ ID
	std::optional<std::string> getString(int id);

	// Lấy giá trị bool từ ID
	std::optional<bool> getBool(int id);
};

void khoitao_giatri();