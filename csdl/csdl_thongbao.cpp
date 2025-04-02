#include "csdl_thongbao.h"
#include <boost/container/string.hpp>

using namespace boost;

void DataManager::addID(const int id)
{
	if (storage.find(id) == storage.end())
	{
		storage[id] = Data{};
	}
}

// Gán giá trị int cho ID
void DataManager::setInt(const int id, int value)
{
	if (storage.find(id) != storage.end())
	{
		storage[id].intValue = value;
	}
}

// Gán giá trị string cho ID
void DataManager::setString(const int id, const std::string& value)
{
	if (storage.find(id) != storage.end())
	{
		storage[id].stringValue = value;
	}
}

// Gán giá trị bool cho ID
void DataManager::setBool(const int id, bool value)
{
	if (storage.find(id) != storage.end())
	{
		storage[id].boolValue = value;
	}
}

// Lấy giá trị int từ ID
std::optional<int> DataManager::getInt(const int id)
{
	if (storage.find(id) != storage.end())
	{
		return storage[id].intValue;
	}
	return std::nullopt;
}

// Lấy giá trị string từ ID
std::optional<std::string> DataManager::getString(const int id)
{
	if (storage.find(id) != storage.end())
	{
		return storage[id].stringValue;
	}
	return std::nullopt;
}

// Lấy giá trị bool từ ID
std::optional<bool> DataManager::getBool(const int id)
{
	if (storage.find(id) != storage.end())
	{
		return storage[id].boolValue;
	}
	return std::nullopt;
}



void khoitao_giatri()
{
	DataManager manager;

	manager.addID(sha256_mismatch);
	manager.setInt(sha256_mismatch, 100);
	manager.setBool(sha256_mismatch, false);
	manager.setString(sha256_mismatch, "Phần mềm không thể cài đặt! Lỗi sha256_mismatch");

	manager.addID(abc);
	manager.setString(abc, "Cảnh báo");
	manager.setInt(abc, 200);
	manager.setBool(abc, false); // Ẩn nút


}


