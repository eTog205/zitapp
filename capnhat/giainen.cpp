// giainen.cpp
#include "giainen.h"
#include "get.h"
#include "log_nhalam.h"

#include <boost/dll.hpp>
#include <boost/process.hpp>

namespace bp = boost::process;
namespace fs = std::filesystem;

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
	const fs::path thumuc_chuongtrinh = fs::path(boost::dll::program_location().parent_path().string());
	const fs::path thumucnen_tuyetdoi = thumuc_chuongtrinh / tentep;

	std::string cmd_line = "\"" + duongdan_winrarexe + "\" x -ibck -y \"" + thumucnen_tuyetdoi.string() + "\" \"" + duongdan_giainen + "\"";

	try
	{
		boost::asio::io_context ctx;
		auto ps = bp::environment::find_executable("powershell.exe", bp::environment::current());

		bp::process proc(ctx.get_executor(), ps, { "-NoProfile", "-Command", cmd_line });

		//bp::process proc(ctx.get_executor(), ps, cmd_line);
		proc.wait();
		const int exit_code = proc.exit_code();

		if (exit_code == 0)
			xoa_tapnen(tentep);

		return (exit_code == 0);
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "Chạy tiến trình thất bại với lỗi: " + std::string(e.what()));
		return false;
	}
}
