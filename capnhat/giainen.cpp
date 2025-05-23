// giainen.cpp
#include "giainen.h"

#include <boost/process.hpp>
#include <boost/process/windows/show_window.hpp>

#include "get.h"
#include "log_nhalam.h"

namespace fs = std::filesystem;
namespace bp = boost::process;

namespace
{
	std::string lay_duongdan_winrar()
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
} // namespace

bool chay_winrar()
{
	try
	{
		fs::path tep_goc = fs::absolute(tentep);
		fs::path thumuc_giainen = tep_goc.parent_path().parent_path();

		// td_log(loai_log::thong_bao, "tệp gốc: " + tep_goc.string());	//debug
		// td_log(loai_log::thong_bao, "thư mục giải nén: " + thumuc_giainen.string());	//debug

		std::string duongdan = lay_duongdan_winrar();
		// td_log(loai_log::thong_bao, "Đường dẫn WinRAR: " + duongdan);	//debug

		std::string cmd_line = "& \"" + duongdan + "\" x -ibck -y \"" + tep_goc.string() + "\" \"" + thumuc_giainen.string() + "\"";

		boost::asio::io_context ctx;
		auto ps = bp::environment::find_executable("powershell.exe", bp::environment::current());

		bp::process proc(ctx.get_executor(), ps, { "-NoProfile", "-Command", cmd_line }, bp::windows::show_window_hide);

		proc.wait();
		const int exit_code = proc.exit_code();

		return (exit_code == 0);
	} catch (std::exception& e)
	{
		td_log(loai_log::loi, "Không chạy được winrar " + std::string(e.what()));
		return false;
	}
}
