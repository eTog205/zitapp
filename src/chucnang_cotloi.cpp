// chucnang_cotloi.cpp
#include "chucnang_cotloi.h"
#include "log_nhalam.h"

//#include <boost/asio/buffer.hpp>
//#include <boost/asio/io_context.hpp>
//#include <boost/asio/read.hpp>
//#include <boost/asio/readable_pipe.hpp>
#include <boost/process.hpp>
#include <boost/process/windows/show_window.hpp>
#include <sstream>

namespace asio = boost::asio;
namespace bp = boost::process;

namespace
{
	std::string thucthi_lenh(const std::string& lenh)
	{
		asio::io_context ctx;
		asio::readable_pipe rp{ ctx };

		std::string output;
		// bool sha256_mismatch = false;
		// bool kotimthaygoi = false;

		try
		{
			auto ps = bp::environment::find_executable("powershell.exe", bp::environment::current());
			bp::process proc(ctx.get_executor(), ps, { "-NoProfile", "-NoExit", "-Command", lenh }, bp::windows::show_window_normal);
			// bp::process proc(ctx, ps, { "-NoProfile", "-Command", lenh }, bp::process_stdio{ /* stdin */ {}, /* stdout */ std::move(rp), /* stderr */ {} }, bp::windows::show_window_normal);

			// boost::system::error_code ec;
			// std::array<char, 4096> buf;

			// while (true)
			//{
			//	std::size_t n = rp.read_some(asio::buffer(buf), ec);
			//	if (n > 0)
			//		output.append(buf.data(), n);
			//	if (ec == asio::error::eof)
			//		break; // đọc đến EOF thì thoát
			//	if (ec)
			//		throw boost::system::system_error(ec);
			// }
			proc.detach();

			/*std::istringstream iss(output);
			std::string line;

			while (std::getline(iss, line))
			{
				if (line.find("No package found matching input criteria") != std::string::npos)
					kotimthaygoi = true;
				if (line.find("Installer hash does not match") != std::string::npos)
					sha256_mismatch = true;
			}*/

			// if (kotimthaygoi)
			//{
			//	return "Package not found";
			// }

			// if (sha256_mismatch)
			//{
			//	td_log(loai_log::canh_bao, "phần mềm được chọn gặp lỗi (sha256_mismatch) cần đợi một thời gian mới có thể cài đặt)");
			// }

			if (proc.exit_code() != 0)
			{
				throw std::runtime_error("Lenh loi voi ma thoat: " + std::to_string(proc.exit_code()));
			}
		} catch (const std::exception& ex)
		{
			td_log(loai_log::loi, "Lỗi xảy ra: " + std::string(ex.what()));
		}

		return output;
	}

	void chay_trong_ps_giu_cuaso(const std::string& lenh)
	{
		try
		{
			asio::io_context ctx;
			auto ps = bp::environment::find_executable("powershell.exe", bp::environment::current());

			bp::process proc(ctx.get_executor(), ps, { "-NoProfile", "-NoExit", "-Command", lenh }, bp::windows::show_window_normal);

			proc.detach();
		} catch (const std::exception& ex)
		{
			td_log(loai_log::loi, "PowerShell wrapper error: " + std::string(ex.what()));
		}
	}

} // namespace

void chaylenh(const std::string& id)
{
	const std::string tuychon_macdinh = " --silent --accept-package-agreements --accept-source-agreements --disable-interactivity";
	const std::string lenh = "winget install " + id + tuychon_macdinh;

	thucthi_lenh(lenh);
}

// cần tìm hiểu cách api hoạt động vì có vẻ là lệnh lấy danh sách ổ đĩa không hoạt động
void chkdsk(std::vector<std::string>& cmds)
{
	try
	{
		td_log(loai_log::thong_bao, "Bắt đầu hàm chkdsk.");

		asio::io_context ctx;
		auto ps = bp::environment::find_executable("powershell", bp::environment::current());

		std::string pw_cmd = "Get-Volume | Where-Object { $_.FileSystem -eq 'NTFS' } | Select-Object -ExpandProperty DriveLetter";

		td_log(loai_log::thong_bao, "Chạy lệnh PowerShell lấy danh sách ổ đĩa NTFS.");

		bp::popen proc(ctx, ps, { "-NoProfile", "-Command", pw_cmd });

		boost::system::error_code ec;
		std::string out;
		char buf[4096];

		while (true)
		{
			std::size_t n = proc.read_some(asio::buffer(buf), ec);
			if (n > 0)
				out.append(buf, buf + n);
			if (ec)
				break;
		}

		td_log(loai_log::thong_bao, "Kết quả từ PowerShell:\n" + out);

		std::istringstream iss(out);
		for (std::string line; std::getline(iss, line);)
		{
			auto f = line.find_first_not_of(" \t\r\n");
			if (f == std::string::npos)
				continue;

			auto l = line.find_last_not_of(" \t\r\n");
			std::string drive = line.substr(f, l - f + 1);

			std::string cmd = "chkdsk " + drive + ": /scan";
			td_log(loai_log::thong_bao, "Tạo lệnh: " + cmd);

			cmds.push_back(cmd);
		}

		proc.wait();

		if (proc.exit_code() != 0)
		{
			td_log(loai_log::loi, "PowerShell thoát với mã lỗi: " + std::to_string(proc.exit_code()));
		}

		td_log(loai_log::thong_bao, "Kết thúc hàm chkdsk.");
	} catch (const std::exception& ex)
	{
		td_log(loai_log::loi, "lỗi hàm chkdsk: " + std::string(ex.what()));
	}
}

void suachua_nhieu(const std::vector<std::string>& commands)
{
	try
	{
		std::string cmd_line;
		for (size_t i = 0; i < commands.size(); ++i)
		{
			if (i > 0)
				cmd_line += "; ";
			cmd_line += commands[i];
		}

		chay_trong_ps_giu_cuaso(cmd_line);
	} catch (const std::exception& ex)
	{
		td_log(loai_log::loi, "Multi-command PowerShell error: " + std::string(ex.what()));
	}
}
