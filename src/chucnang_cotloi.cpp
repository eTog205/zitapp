// chucnang_cotloi.cpp
#include "chucnang_cotloi.h"
#include "log_nhalam.h"

#include <boost/process.hpp>
#include <boost/process/v1/windows.hpp>

namespace bp = boost::process;

namespace
{
	std::string thucthi_lenh(const std::string& lenh)
	{
		std::ostringstream output;
		try
		{
			bool sha256_mismatch = false;
			bp::ipstream pipe_stream;
			bp::child process(lenh, bp::std_out > pipe_stream);
			std::string line;
			bool kotimthaygoi = false;

			while (pipe_stream && getline(pipe_stream, line))
			{
				output << line << "\n";

				if (line.find("No package found matching input criteria") != std::string::npos)
					kotimthaygoi = true;

				if (line.find("Installer hash does not match") != std::string::npos)
					sha256_mismatch = true;
			}

			process.wait();

			if (kotimthaygoi)
			{
				return "Package not found";
			}

			if (sha256_mismatch)
			{
				td_log(loai_log::canh_bao, "phần mềm được chọn gặp lỗi (sha256_mismatch) cần đợi một thời gian mới có thể cài đặt)");
			}

			if (process.exit_code() != 0)
			{
				throw std::runtime_error("Lenh loi voi ma thoat: " + std::to_string(process.exit_code()));
			}
		} catch (const std::exception& ex)
		{
			td_log(loai_log::loi, "Lỗi xảy ra: " + std::string(ex.what()));
		}

		return output.str();
	}

	void chay_trong_ps_giu_cua_so(const std::string& lenh)
	{
		try
		{
			bp::child proc(bp::search_path("powershell"), bp::args({ "-NoExit", "-Command", lenh }), bp::windows::show);
			proc.detach();
		} catch (const std::exception& ex)
		{
			td_log(loai_log::loi, "PowerShell wrapper: " + std::string(ex.what()));
		}
	}

} // namespace

void chaylenh(const std::string& id)
{
	const std::string tuychon_macdinh = " --silent --accept-package-agreements --accept-source-agreements --disable-interactivity";
	const std::string lenh = "winget install " + id + tuychon_macdinh;

	thucthi_lenh(lenh);
}

void chkdsk(std::vector<std::string>& cmds)
{
	try
	{
		bp::ipstream pipe;

		auto ps = bp::search_path("powershell");
		bp::child c(ps, bp::args({ "-NoProfile", "-Command", "Get-Volume | Where-Object { $_.FileSystem -eq 'NTFS' } | Select-Object -ExpandProperty DriveLetter" }), bp::std_out > pipe,
					bp::windows::hide);

		std::string line;
		while (pipe && std::getline(pipe, line))
		{
			auto f = line.find_first_not_of(" \t\r\n");
			auto l = line.find_last_not_of(" \t\r\n");
			if (f == std::string::npos)
				continue;
			std::string d = line.substr(f, l - f + 1);
			if (!d.empty())
				cmds.push_back("chkdsk " + d + ": /scan");
		}
		c.wait();
	} catch (const std::exception& ex)
	{
		td_log(loai_log::loi, "lấy danh sách ổ đĩa cho chkdsk: " + std::string(ex.what()));
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

		chay_trong_ps_giu_cua_so(cmd_line);
	} catch (const std::exception& ex)
	{
		td_log(loai_log::loi, "Multi-command PowerShell error: " + std::string(ex.what()));
	}
}
