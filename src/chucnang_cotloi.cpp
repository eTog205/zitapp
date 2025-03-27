//chucnang_coban.cpp
#include "chucnang_cotloi.h"
#include "log_nhalam.h"

std::string thucthi_lenh(const std::string& lenh)
{
	std::ostringstream output;
	try
	{
		bool sha256_mismatch = false;
		bp::ipstream pipe_stream; // Luồng để nhận đầu ra từ lệnh
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

		process.wait(); // Chờ lệnh kết thúc

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

void chaylenh(const std::string& id)
{
	const std::string tuychon_macdinh = " --silent --accept-package-agreements --accept-source-agreements --disable-interactivity";
	const std::string lenh = "winget install " + id + tuychon_macdinh;

	thucthi_lenh(lenh);
}

void chaylenh_suawindow()
{
	try
	{
		std::string powershell_path = R"(C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe)";
		//std::string powershell_path = R"(C:\Program Files\PowerShell\7\pwsh.exe)";

		std::string command =
			"Start-Process -FilePath 'powershell.exe' "
			"-ArgumentList '-NoExit', '-ExecutionPolicy Bypass', "
			"'sfc /scannow; Dism /Online /Cleanup-Image /RestoreHealth; sfc /scannow' "
			"-Verb RunAs";

		/*std::string command =
			"Start-Process -FilePath 'pwsh.exe' "
			"-ArgumentList '-NoExit','-ExecutionPolicy','Bypass','-Command','sfc /scannow; Dism /Online /Cleanup-Image /RestoreHealth; sfc /scannow' "
			"-Verb RunAs";*/

		bp::child process(powershell_path, bp::args = { "-NoExit","-NoProfile", "-ExecutionPolicy", "Bypass","-WindowStyle", "Hidden", "-Command", command });
		process.wait();
	} catch (const std::exception& ex)
	{
		td_log(loai_log::loi, "Lỗi: " + std::string(ex.what()));
	}
}


