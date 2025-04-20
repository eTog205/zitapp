//giainen.cpp
#include "get.h"
#include "giainen.h"
#include "log_nhalam.h"

#include <boost/dll.hpp>
#include <boost/process.hpp>
#include <windows.h>

namespace bp = boost::process;
namespace bfs = boost::filesystem;

void xoa_tapnen(const std::string& file_path)
{
	if (std::remove(file_path.c_str()) != 0)
		td_log(loai_log::loi, "Xóa tệp nén thất bại: " + file_path);
}

std::string lau_duongdan_winrar()
{
	HKEY h_key;
	char buffer[512]{};
	DWORD dw_size = sizeof(buffer);

	// Thử mở key "SOFTWARE\WinRAR"
	LONG l_res = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\WinRAR", 0, KEY_READ, &h_key);
	if (l_res != ERROR_SUCCESS)
		td_log(loai_log::loi, "Không mở được key SOFTWARE_WinRAR");

	l_res = RegQueryValueExA(h_key, "exe64", nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer), &dw_size);
	RegCloseKey(h_key);
	if (l_res != ERROR_SUCCESS)
	{
		td_log(loai_log::loi, "Không đọc được key exe64");
		return "";
	}

	return { buffer };
}

bool chay_winrar(const std::string& duongdan_winrarexe, const std::string& duongdan_giainen)
{
	const bfs::path thumuc_chuongtrinh = boost::dll::program_location().parent_path();
	const bfs::path thumucnen_tuyetdoi = thumuc_chuongtrinh / tentep;

	std::string cmd_line = "\"" + duongdan_winrarexe + "\" x -ibck -y \"" + thumucnen_tuyetdoi.string() + "\" \"" + duongdan_giainen + "\"";

	try
	{
		bp::child c(cmd_line);
		c.wait();
		const int exit_code = c.exit_code();

		if (exit_code == 0)
			xoa_tapnen(tentep);

		return (exit_code == 0);
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "Chạy tiến trình thất bại với lỗi: " + std::string(e.what()));
		return false;
	}
}


