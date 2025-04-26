// logic_giaodien.cpp
#include "logic_giaodien.h"
#include "chay_luongphu.h"
#include "chucnang_cotloi.h"
#include "giaodien.h"
#include "log_nhalam.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

logic_giaodien lg_gd;
dulieuduongdan dl;
cauhinh ch;

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

using demtg = std::chrono::steady_clock;

void logic_giaodien::khoidong_bang_dl()
{
	ch_b.columns.clear();
	ch_b.add_column(column_config("chon", "##Chọn", 40.0f, 40.0f, 40.0f, false, true, false));
	ch_b.add_column(column_config("id", "ID", 80.0f, 50.0f, 200.0f, true, false));
	ch_b.add_column(column_config("ten", "Tên", 150.0f, 80.0f, 300.0f));
	ch_b.add_column(column_config("phanloai", "Phân loại", 120.0f, 60.0f, 250.0f));
}

std::string wstring_to_string(const std::wstring& wch)
{
	if (wch.empty())
		return "";

	const int size_needed = WideCharToMultiByte(CP_UTF8, 0, wch.c_str(), static_cast<int>(wch.size()), nullptr, 0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wch.c_str(), static_cast<int>(wch.size()), str.data(), size_needed, nullptr, nullptr);

	return str;
}

std::wstring string_to_wstring(const std::string& chuoi)
{
	if (chuoi.empty())
		return L"";

	const int size_needed = MultiByteToWideChar(CP_UTF8, 0, chuoi.c_str(), static_cast<int>(chuoi.size()), nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, chuoi.c_str(), static_cast<int>(chuoi.size()), wstr.data(), size_needed);

	return wstr;
}

float tt_thugonkichthuoc(bool& da_thugon, bool& yeucau_thugon, const std::chrono::steady_clock::time_point& thoigian_batdau_thugon, const float kichthuoc_morong, const float kichthuoc_thugon,
						 const float thoigian_tre)
{
	float new_size = da_thugon ? kichthuoc_thugon : kichthuoc_morong;
	if (yeucau_thugon)
	{
		const auto now = std::chrono::steady_clock::now();
		const float elapsed = std::chrono::duration<float>(now - thoigian_batdau_thugon).count();

		const float t = std::clamp(elapsed / thoigian_tre, 0.0f, 1.0f);

		const float start_size = da_thugon ? kichthuoc_thugon : kichthuoc_morong;
		const float target_size = da_thugon ? kichthuoc_morong : kichthuoc_thugon;
		new_size = start_size + t * (target_size - start_size);

		if (t >= 1.0f)
		{
			da_thugon = !da_thugon;
			yeucau_thugon = false;
		}
	}

	return new_size;
}

std::string tt_vanbancothenhinthay(const std::wstring& toanbo_vanban, const float chieurong_hientai, const float chieurong_toithieu, const float chieurong_toida)
{
	const float ratio = std::clamp((chieurong_hientai - chieurong_toithieu) / (chieurong_toida - chieurong_toithieu), 0.0f, 1.0f);
	const int max_chars = std::clamp(static_cast<int>(ratio * static_cast<float>(toanbo_vanban.size())), 1, static_cast<int>(toanbo_vanban.size()));
	return wstring_to_string(toanbo_vanban.substr(0, max_chars));
}

void logic_giaodien::chaylenh_winget(const std::string& lenh_id)
{
	lp_chay_lenhwinget(lenh_id);
}

void tao_thumuc_tainguyen()
{
	if (!exists(ch.thumuc_ch))
	{
		create_directory(ch.thumuc_ch);
	}
}

void saochep_ini()
{
	tao_thumuc_tainguyen();
	try
	{
		boost::property_tree::ptree pt;
		read_ini(ch.tep_nguon.string(), pt);
		write_ini(ch.tep_dich.string(), pt);
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, e.what());
	}
}

bool kiemtra_duongdan(const std::string& duongdan_str, bool (*ham_kiemtra)(const fs::path&), const char* thongbao_loi, const bool chophep_rong, std::string& loi)
{
	const fs::path duongdan = duongdan_str;

	if (!chophep_rong && duongdan.empty())
	{
		loi = "Lỗi: Đường dẫn không được rỗng";
		return false;
	}

	if (!duongdan.empty() && !ham_kiemtra(duongdan))
	{
		loi = thongbao_loi;
		return false;
	}

	if (ham_kiemtra == static_cast<bool (*)(const fs::path&)>(fs::is_regular_file))
	{
		std::string ext = duongdan.extension().string();
		std::ranges::transform(ext, ext.begin(), [](const unsigned char c) { return std::tolower(c); });
		if (ext != ".ini")
		{
			loi = "Lỗi: Tệp không phải định dạng .ini!";
			return false;
		}
	}

	loi.clear();
	return true;
}

bool kiemtraduongdan_thumuc()
{
	const bool dung = kiemtra_duongdan(dl.dd_xuat, fs::is_directory, "Lỗi: Đường dẫn thư mục không hợp lệ!", true, dl.loi_xuat_tepch);
	if (!dung)
	{
		dl.thoigian_hienthi_loi_xuat = demtg::now();
	}
	return dung;
}

bool kiemtraduongdan_taptin()
{
	const bool dung = kiemtra_duongdan(dl.dd_nap, fs::is_regular_file, "Lỗi: Đường dẫn tệp tin không hợp lệ!", false, dl.loi_nap_tepch);
	if (!dung)
	{
		dl.thoigian_hienthi_loi_nap = demtg::now();
	}
	return dung;
}

void xuat_cauhinh(const std::string& duongdan_xuat)
{
	boost::property_tree::ptree pt;

	pt.put("menuben_thugon", gd.menuben_thugon);
	pt.put("thoigian_thugon", gd.thoigian_thugon);
	pt.put("letrai_bang", gd.letrai_bang);
	pt.put("letren_bang", gd.letren_bang);
	pt.put("chieurong_menuben_thugon", gd.chieurong_menuben_thugon);
	pt.put("chieurong_menuben_morong", gd.chieurong_menuben_morong);

	write_ini(duongdan_xuat, pt);
}

void nap_cauhinh()
{
	if (exists(ch.tep_dich))
	{
		boost::property_tree::ptree pt;
		read_ini(ch.tep_dich.string(), pt);

		capnhat_cauhinh(pt, "menuben_thugon", gd.menuben_thugon);
		capnhat_cauhinh(pt, "thoigian_thugon", gd.thoigian_thugon);
		capnhat_cauhinh(pt, "letrai_bang", gd.letrai_bang);
		capnhat_cauhinh(pt, "letren_bang", gd.letren_bang);
		capnhat_cauhinh(pt, "chieurong_menuben_thugon", gd.chieurong_menuben_thugon);
		capnhat_cauhinh(pt, "chieurong_menuben_morong", gd.chieurong_menuben_morong);
	}
}

void ch_macdinh()
{
	fs::remove(ch.tep_dich);
}

// không còn được hỗ trợ - xóa ở cập nhật sau
//bool kiemtra_khoapro(const std::string& key, std::string& ngayhethan)
//{
//
//	const std::string host = "api.tntstudio.io.vn";
//	const std::string port = "443";
//	const std::string target = "/";
//	constexpr int version = 11;
//
//	net::io_context ioc;
//	boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23_client);
//	beast::ssl_stream<tcp::socket> stream(ioc, ctx);
//
//	tcp::resolver resolver(ioc);
//	auto const results = resolver.resolve(host, port);
//
//	net::connect(stream.next_layer(), results);
//
//	if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
//	{
//		boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
//		throw boost::system::system_error{ ec };
//	}
//
//	stream.handshake(boost::asio::ssl::stream_base::client);
//
//	boost::json::object body_obj;
//	body_obj["key"] = key;
//	std::string body_str = serialize(body_obj);
//
//	http::request<http::string_body> req{ http::verb::post, target, version };
//	req.set(http::field::host, host);
//	req.set(http::field::user_agent, "ZitApp/1.0");
//	req.set(http::field::content_type, "application/json");
//	req.body() = body_str;
//	req.prepare_payload();
//
//	http::write(stream, req);
//
//	beast::flat_buffer buffer;
//	http::response<http::string_body> res;
//	http::read(stream, buffer, res);
//
//	// Đóng SSL
//	boost::system::error_code ec;
//	stream.shutdown(ec);
//	if (ec == boost::asio::error::eof || ec == boost::asio::ssl::error::stream_truncated)
//	{
//		// Cloudflare đóng sớm – bỏ qua
//		ec.clear();
//	}
//	else if (ec)
//	{
//		throw boost::system::system_error{ ec };
//	}
//
//	// Phân tích JSON phản hồi
//	auto json_res = boost::json::parse(res.body()).as_object();
//	if (json_res.contains("valid") && json_res.at("valid").as_bool())
//	{
//		ngayhethan = json_res.at("ngayhethan").as_string().c_str();
//		return true;
//	}
//
//	return false;
//}
