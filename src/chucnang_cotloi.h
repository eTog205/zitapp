//chucnang_cotloi.h
#pragma once
#include <boost/process.hpp>

namespace bp = boost::process;

// Hàm thực thi lệnh hệ thống và lấy kết quả
std::string thucthi_lenh(const std::string& lenh);

void chaylenh(const std::string& id);
void chaylenh_suawindow();