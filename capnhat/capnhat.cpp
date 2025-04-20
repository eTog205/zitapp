// capnhat.cpp
#include "capnhat.h"
#include "get.h"
#include "giainen.h"
#include "log_nhalam.h"
#include "net.h"

#include <boost/dll.hpp>
#include <boost/process.hpp>

using namespace std::chrono;
namespace bfs = boost::filesystem;

std::string get_appcoban_path()
{
	thongtin tt;
	const bfs::path duongdan_capnhat = boost::dll::program_location();
	const bfs::path thumuc_capnhat = duongdan_capnhat.parent_path();
	const bfs::path thumuc_cha = thumuc_capnhat.parent_path();
	const bfs::path duongdan_appcb = thumuc_cha / tt.tentep_appchinh;
	return duongdan_appcb.string();
}

std::string doctep_phienban(const std::string& tep_phienban)
{
	std::ifstream tep(tep_phienban);
	std::string dong;

	if (!tep.is_open())
	{
		td_log(loai_log::canh_bao, "Không thể mở tệp để đọc " + std::string(tep_phienban));
		return dong;
	}

	getline(tep, dong);

	tep.close();
	return dong;
}

void ghitep_phienban(const std::string& tep_phienban, const std::string& dl_canghi)
{
	std::ofstream tep(tep_phienban);

	if (!tep.is_open())
	{
		td_log(loai_log::canh_bao, "mở tệp để ghi thất bại." + std::string(tep_phienban));
		return;
	}

	tep << dl_canghi;
	tep.close();
}

int loc_dl(const std::string& dauvao)
{
	std::string kq;
	for (const char ch : dauvao)
	{
		if (isdigit(ch))
			kq.push_back(ch);
	}

	if (kq.empty())
		return 0;

	try
	{
		return std::stoi(kq);
	} catch (const std::invalid_argument& e)
	{
		td_log(loai_log::loi, "không phải là số hợp lệ: " + std::string(e.what()));
	} catch (const std::out_of_range& e)
	{
		td_log(loai_log::loi, "vượt quá giới hạn kiểu int" + std::string(e.what()));
	}

	return 0;
}

std::string dinhdang_dl(const int so_phienban)
{
	std::ostringstream oss;
	oss << std::setw(3) << std::setfill('0') << so_phienban;

	const std::string chuoi_sau_kiemtra = oss.str();
	std::string chuoi_ketqua = std::string(1, chuoi_sau_kiemtra[0]) + "." + std::string(1, chuoi_sau_kiemtra[1]) + "." + std::string(1, chuoi_sau_kiemtra[2]);

	return chuoi_ketqua;
}

bool sosanh_phienban(const int so_hientai, const int so_layve)
{
	return so_hientai < so_layve;
}

void kiemtra_capnhat()
{
	thongtin tt;
	const std::string latest_tag = get_release_tag();
	if (latest_tag.empty())
	{
		td_log(loai_log::canh_bao, "Không thể lấy phiên bản mới nhất!");
		chay_app_co_ban();
		return;
	}

	const std::string dl_doc = doctep_phienban(tt.tentep_phienban);
	const int so_hientai_daloc = loc_dl(dl_doc);
	const int so_layve_daloc = loc_dl(latest_tag);

	if (sosanh_phienban(so_hientai_daloc, so_layve_daloc))
	{
		td_log(loai_log::thong_bao, "Có bản cập nhật mới: " + latest_tag);

		if (download_latest_release())
		{
			const std::string dl_doi_dinhdang = dinhdang_dl(so_layve_daloc);
			ghitep_phienban(tt.tentep_phienban, dl_doi_dinhdang);

			capnhat();
			td_log(loai_log::thong_bao, "Hoàn tất cập nhật phiên bản mới " + dl_doi_dinhdang);
			//td_log(loai_log::thong_bao, "Chạy " + tt.tentep_appchinh + " " + dl_doi_dinhdang);	//debug
			chay_app_co_ban();
		}
		else
			td_log(loai_log::loi, "Không thể tải xuống bản cập nhật!");
	}
	else
	{
		// td_log(loai_log::thong_bao, "Phiên bản hiện tại (" + dinhdang_dl(so_hientai_daloc) + ") đã mới nhất.");	//debug
		chay_app_co_ban();
	}
}

void capnhat()
{
	const std::string duongdan_winrar = lau_duongdan_winrar();
	const std::string app_path = get_appcoban_path();
	const bfs::path dest_path(app_path);

	const std::string duongdan_thumuc_giainen = dest_path.parent_path().string();

	if (chay_winrar(duongdan_winrar, duongdan_thumuc_giainen))
	{
		//td_log(loai_log::thong_bao, "Chạy tiến trình Winrar thành công");	//debug
	}
	else
	{
		td_log(loai_log::loi, "Chạy tiến trình Winrar thất bại");
	}
}

void chay_app_co_ban()
{
	try
	{
		const std::string app_path = get_appcoban_path();

		const bfs::path app_dir = bfs::path(app_path).parent_path();
		boost::process::child process(app_path, boost::process::start_dir = app_dir.string());

		process.wait();
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "Không chạy được zitapp.exe:" + std::string(e.what()));
	}
}
