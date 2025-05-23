// capnhat.cpp
#include "capnhat.h"

#include <boost/dll.hpp>
#include <boost/process.hpp>

#include "get.h"
#include "giainen.h"
#include "kiemtra_ketnoi_mang.h"
#include "log_nhalam.h"
#include "net.h"

using namespace std::chrono;
namespace bp = boost::process;
namespace fs = std::filesystem;

std::string lay_duongdan_zitapp()
{
	thongtin tt;
	const fs::path duongdan_capnhat = fs::path(boost::dll::program_location().string());
	const fs::path thumuc_capnhat = duongdan_capnhat.parent_path();
	const fs::path thumuc_cha = thumuc_capnhat.parent_path();
	const fs::path duongdan_appcb = thumuc_cha / tt.tentep_appchinh;
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

void capnhat()
{
	if (chay_winrar())
	{
		// td_log(loai_log::thong_bao, "Chạy tiến trình Winrar thành công");	//debug
	}
	else
		td_log(loai_log::loi, "Chạy tiến trình Winrar thất bại");
}

void chay_zitapp()
{
	try
	{
		const std::string ap = lay_duongdan_zitapp();
		fs::path ad = fs::path(ap).parent_path();
		boost::asio::io_context ctx;
		bp::process proc(ctx, ap, {}, bp::process_start_dir(ad));
		proc.wait();
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "Không chạy được zitapp.exe:" + std::string(e.what()));
	}
}

void kiemtra_capnhat()
{
	if (!kiemtra_trangthai_mang())
	{
		// td_log(loai_log::canh_bao, "Không có kết nối mạng. Bỏ qua cập nhật");	//debug
		chay_zitapp();
		return;
	}

	thongtin tt;
	const std::string latest_tag = get_release_tag();
	if (latest_tag.empty())
	{
		td_log(loai_log::canh_bao, "Không thể lấy phiên bản mới nhất!");
		chay_zitapp();
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

			// td_log(loai_log::thong_bao, "Chạy " + tt.tentep_appchinh + " " + dl_doi_dinhdang);	//debug
			chay_zitapp();

			if (std::remove(tentep.c_str()) != 0)
				td_log(loai_log::loi, "Xóa tệp nén thất bại: " + tentep);
		}
		else
			td_log(loai_log::loi, "Không thể tải xuống bản cập nhật!");
	}
	else
	{
		// td_log(loai_log::thong_bao, "Phiên bản hiện tại (" + dinhdang_dl(so_hientai_daloc) + ") đã mới nhất.");	//debug
		chay_zitapp();
	}
}
