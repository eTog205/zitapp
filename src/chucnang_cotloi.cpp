// chucnang_cotloi.cpp
#include "chucnang_cotloi.h"

#include <boost/process.hpp>
#include <boost/process/windows/show_window.hpp>
#include <unordered_set>
#include <vector>

#include "log_nhalam.h"

namespace asio = boost::asio;
namespace bp = boost::process;

const std::unordered_set<std::wstring> phan_morong_boqua = { L".evtx", L".lock" };

namespace
{
	// chức năng tải và cài đặt (winget)
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
			proc.detach(); // dùng wait

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

	// chức năng sửa
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

	std::vector<std::string> lay_o_ntfs()
	{
		std::vector<std::string> danh_sach;

		DWORD mask = GetLogicalDrives();
		for (char c = 'A'; c <= 'Z'; ++c)
		{
			if (!(mask & (1 << (c - 'A'))))
				continue;

			std::string o = std::string(1, c) + ":\\";

			char he_thong_tap_tin[MAX_PATH] = { 0 };
			if (GetVolumeInformationA(o.c_str(), nullptr, 0, nullptr, nullptr, nullptr, he_thong_tap_tin, sizeof(he_thong_tap_tin)))
			{
				if (_stricmp(he_thong_tap_tin, "NTFS") == 0)
					danh_sach.emplace_back(1, c);
			}
		}
		return danh_sach;
	}

	// chức năng xóa
	bool nen_boqua(const fs::path& duongdan)
	{
		return phan_morong_boqua.contains(duongdan.extension());
	}

	void xoa_dulieu_trongthumuc(const fs::path& duongdan, chedo cheDo, ThongKe* thongKe = nullptr)
	{
		try
		{
			if (!fs::exists(duongdan))
				return;

			for (const auto& muc : fs::recursive_directory_iterator(duongdan))
			{
				try
				{
					if (nen_boqua(muc.path()))
					{
						if (thongKe)
						{
							thongKe->biBoQua++;
							thongKe->daTimThay++;
						}
						continue;
					}

					uintmax_t kichThuoc = fs::is_regular_file(muc) ? fs::file_size(muc) : 0;
					uintmax_t kichThuocMB = kichThuoc / (1024 * 1024);

					if (cheDo == chedo::theodoi)
					{
						if (thongKe)
						{
							thongKe->tongDungLuongDaXoa += kichThuocMB;
							thongKe->daTimThay++;
						}
						continue;
					}

					std::error_code ec;
					if (fs::is_directory(muc))
						fs::remove_all(muc, ec);
					else
						fs::remove(muc, ec);

					if (!ec)
					{
						if (thongKe)
						{
							thongKe->daXoa++;
							thongKe->tongDungLuongDaXoa += kichThuocMB;
							thongKe->daTimThay++;
						}
					}
					else
					{
						if (thongKe)
						{
							thongKe->biBoQua++;
							thongKe->daTimThay++;
						}
					}
				} catch (...)
				{
					if (thongKe)
					{
						thongKe->biBoQua++;
						thongKe->daTimThay++;
					}
				}
			}
		} catch (...)
		{
			// bỏ qua toàn bộ lỗi thư mục gốc
		}
	}

	void tienhanh_xoa(const std::vector<fs::path>& danhSach, chedo cheDo, ThongKe* thongKe = nullptr)
	{
		for (const auto& duongdan : danhSach)
		{
			xoa_dulieu_trongthumuc(duongdan, cheDo, thongKe);
		}
	}

	std::wstring lay_bien_moitruong(const std::wstring& tenBien)
	{
		std::wstring ketQua;
		DWORD kichThuoc = GetEnvironmentVariableW(tenBien.c_str(), nullptr, 0);
		if (kichThuoc == 0)
			return L"";
		ketQua.resize(kichThuoc);
		GetEnvironmentVariableW(tenBien.c_str(), ketQua.data(), kichThuoc);
		ketQua.resize(kichThuoc - 1);
		return ketQua;
	}

	std::vector<mucdich_xoa> lay_nguon_duongdan_macdinh()
	{
		std::wstring oHeThong = lay_bien_moitruong(L"SYSTEMDRIVE");
		std::wstring thuMucWin = lay_bien_moitruong(L"WINDIR");
		std::wstring appLocal = lay_bien_moitruong(L"LOCALAPPDATA");

		return { { lay_bien_moitruong(L"TEMP"), nhom::khuyennghi },
				 { thuMucWin + L"\\Temp", nhom::khuyennghi },
				 { thuMucWin + L"\\Logs\\CBS", nhom::khuyennghi },
				 { appLocal + L"\\Temp", nhom::khuyennghi },
				 { appLocal + L"\\Microsoft\\Windows\\Explorer", nhom::khuyennghi },

				 // tùy chỉnh
				 { oHeThong + L"\\Windows\\Prefetch", nhom::tuychon },
				 { oHeThong + L"\\Windows\\SoftwareDistribution\\Download", nhom::tuychon },
				 { thuMucWin + L"\\System32\\LogFiles", nhom::tuychon },
				 { thuMucWin + L"\\System32\\winevt\\Logs", nhom::tuychon } };
	}

	std::vector<fs::path> loc_duongdanthucte(const std::vector<mucdich_xoa>& danhSach, const cauhinh_xoa& cauHinh)
	{
		std::vector<fs::path> ketQua;
		for (const auto& muc : danhSach)
		{
			if (!cauHinh.batNhom.at(muc.nhom))
				continue;

			bool trung = false;
			for (const auto& daCo : ketQua)
			{
				try
				{
					if (fs::exists(muc.duongdan) && fs::exists(daCo) && fs::equivalent(muc.duongdan, daCo))
					{
						trung = true;
						break;
					}
				} catch (...)
				{}
			}
			if (!trung)
				ketQua.push_back(muc.duongdan);
		}
		return ketQua;
	}

} // namespace

// === BẢNG DỮ LIỆU ===
// chức năng tải về và cài đặt (winget)
void chaylenh(const std::string& id)
{
	const std::string tuychon_macdinh = " --silent --accept-package-agreements --accept-source-agreements --disable-interactivity";
	const std::string lenh = "winget install " + id + tuychon_macdinh;

	thucthi_lenh(lenh);
}

// === TIỆN ÍCH ===
// chức năng sửa lỗi
std::vector<std::string> tao_lenh_chkdsk()
{
	const auto ds_o = lay_o_ntfs();
	std::vector<std::string> lenh;
	lenh.reserve(ds_o.size());

	for (const auto& o : ds_o)
		lenh.emplace_back("chkdsk " + o + ": /scan");

	return lenh;
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
		td_log(loai_log::loi, "Nhiều lệnh PS: " + std::string(ex.what()));
	}
}

// cần sửa: xóa không sạch
// chức năng xóa rác
void tienhanh_xoa()
{
	cauhinh_xoa cauhinh;
	auto danhSach = loc_duongdanthucte(lay_nguon_duongdan_macdinh(), cauhinh);
	tienhanh_xoa(danhSach, chedo::imlang, nullptr);
}

// chức năng phân mảng
bool mo_phanmanh()
{
	try
	{
		asio::io_context ctx;
		std::string cmd_line = "Start-Process dfrgui";
		auto ps = bp::environment::find_executable("powershell.exe", bp::environment::current());
		bp::process proc(ctx.get_executor(), ps, { "-NoProfile", "-Command", cmd_line }, bp::windows::show_window_hide);

		proc.wait();

		return proc.exit_code();
	} catch (const std::exception& ex)
	{
		td_log(loai_log::loi, ex.what());
		return false;
	}
}

bool chay_phanmanh()
{
	try
	{
		asio::io_context ctx;

		auto ds_o = lay_o_ntfs();

		for (const auto& o : ds_o)
		{
			std::string cmd_line = "defrag " + o + ": -w -v";
			auto ps = bp::environment::find_executable("powershell.exe", bp::environment::current());

			bp::process proc(ctx.get_executor(), ps, { "-NoProfile", "-Command", cmd_line }, bp::windows::show_window_hide);

			proc.wait();

			if (proc.exit_code() != 0)
			{
				td_log(loai_log::loi, "Lỗi khi phân mảnh ổ " + o);
				return false;
			}

			// td_log(loai_log::thong_bao, "Đã phân mảnh xong ổ " + o);
		}

		return true;
	} catch (const std::exception& ex)
	{
		td_log(loai_log::loi, ex.what());
		return false;
	}
}
