#include "net.h"
#include "log_nhalam.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
namespace bj = boost::json;
namespace ssl = asio::ssl;
namespace json = boost::json;
using tcp = asio::ip::tcp;

std::string gui_http_request(const thamso_http& ts)
{
	if (!ts.url.starts_with("https://"))
	{
		td_log(loai_log::loi, "[http_request] URL không hợp lệ: " + ts.url);
		return "";
	}

	auto url_body = ts.url.substr(strlen("https://"));
	auto pos = url_body.find('/');
	if (pos == std::string::npos)
	{
		td_log(loai_log::loi, "[http_request] URL không có target hợp lệ: " + ts.url);
		return "";
	}
	std::string host = url_body.substr(0, pos);
	std::string target = url_body.substr(pos);

	try
	{
		asio::io_context ioc;
		ssl::context ctx{ ssl::context::tlsv12_client };
		ctx.set_default_verify_paths();

		beast::ssl_stream<beast::tcp_stream> stream{ ioc, ctx };
		tcp::resolver resolver{ ioc };
		auto const results = resolver.resolve(host, macdinh_port);
		beast::get_lowest_layer(stream).connect(results);

		if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
		{
			beast::error_code ec{ static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category() };
			td_log(loai_log::loi, "SNI error: " + ec.message());
			throw beast::system_error{ ec };
		}
		stream.handshake(ssl::stream_base::client);

		http::request<http::string_body> req{ ts.method == "POST" ? http::verb::post : http::verb::get, target, 11 };
		if (ts.method == "POST")
		{
			req.body() = ts.body;
			req.set(http::field::content_type, "application/json");
			req.prepare_payload();
		}
		req.set(http::field::host, host);
		req.set(http::field::user_agent, ts.user_agent);
		req.set(http::field::accept, ts.is_asset ? "application/octet-stream" : ts.accept);

		http::write(stream, req);

		beast::flat_buffer buffer;
		http::response_parser<http::dynamic_body> parser;
		parser.body_limit(cauhinh_chung{}.gioihan_body);
		http::read(stream, buffer, parser);
		auto& res = parser.get();

		if (ts.cho_phep_redirect && res.result_int() >= 300 && res.result_int() < 400)
		{
			if (ts.so_lan_redirect >= 5)
			{
				td_log(loai_log::loi, "Quá số lần redirect cho phép.");
				return "";
			}
			if (auto it = res.find(http::field::location); it != res.end())
			{
				thamso_http ts2 = ts;
				ts2.url = std::string(it->value().data(), it->value().size());
				ts2.so_lan_redirect++;
				return gui_http_request(ts2);
			}
			td_log(loai_log::loi, "Không tìm thấy Location trong redirect.");
			return "";
		}

		if (res.result() != http::status::ok)
		{
			td_log(loai_log::loi, "HTTP lỗi: " + std::to_string(res.result_int()));
			return "";
		}

		beast::error_code ec;
		stream.next_layer().socket().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
		stream.next_layer().socket().close(ec);

		return beast::buffers_to_string(res.body().data());
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, std::string("HTTP exception: ") + e.what());
		return "";
	}
}

std::string gui_post_api(const std::string& noidung_json)
{
	cauhinh_api ap;
	thamso_http ts;
	ts.url = std::string("https://") + ap.host + ap.target;
	ts.method = "POST";
	ts.body = noidung_json;
	ts.accept = ap.accept;
	ts.cho_phep_redirect = false;
	return gui_http_request(ts);
}

std::string tai_raw_github_file(const std::string& target)
{
	cauhinh_github gh;
	thamso_http ts;
	ts.url = std::string("https://") + gh.host + target;
	ts.accept = gh.accept_raw;
	return gui_http_request(ts);
}

std::string tai_url_http(const std::string& url, bool is_asset, int redirect_count)
{
	thamso_http ts;
	ts.url = url;
	ts.is_asset = is_asset;
	ts.so_lan_redirect = redirect_count;
	return gui_http_request(ts);
}

std::string lay_thongtin_github(const std::string& file_path)
{
	cauhinh_github gh;
	std::string target = "/repos/" + gh.owner + "/" + gh.repo_tainguyen + "/contents/" + file_path;
	std::string res = tai_raw_github_file(target);
	if (res.empty())
		td_log(loai_log::loi, "Không tải dữ liệu từ GitHub");
	return res;
}

std::string lay_thongtin_tep_github(const std::string& file_path)
{
	cauhinh_github gh;
	std::string target = "/repos/" + gh.owner + "/" + gh.repo_tainguyen + "/commits?path=" + file_path;
	thamso_http ts;
	ts.url = std::string("https://") + gh.host + target;
	ts.accept = gh.accept_json;
	ts.method = "GET";
	ts.cho_phep_redirect = false;
	std::string res = gui_http_request(ts);
	if (res.empty())
		td_log(loai_log::loi, "Không lấy metadata GitHub");
	return res;
}

std::string get_release_tag()
{
	cauhinh_github gh;
	std::string url = "https://" + gh.host + "/repos/" + gh.owner + "/" + gh.repo_app + "/releases/latest";
	thamso_http ts;
	ts.url = url;
	ts.accept = gh.accept_json;
	ts.method = "GET";
	ts.cho_phep_redirect = false;
	std::string body = gui_http_request(ts);
	if (body.empty())
	{
		td_log(loai_log::loi, "[get_release_tag] Không phản hồi từ GitHub API");
		return "";
	}
	auto jv = json::parse(body).as_object();
	if (!jv.contains("tag_name") || !jv.at("tag_name").is_string())
	{
		td_log(loai_log::canh_bao, "Không tìm thấy tag_name");
		return "";
	}
	std::string tag = json::value_to<std::string>(jv["tag_name"]);

	//td_log(loai_log::thong_bao, "Phiên bản mới nhất: " + tag);	//debug
	return tag;
}

std::optional<ketqua_key> kiemtra_key_online(const std::string& key)
{
	bj::object obj;
	obj["key"] = key;
	std::string resp = gui_post_api(bj::serialize(obj));
	if (resp.empty())
		return std::nullopt;
	try
	{
		auto root = bj::parse(resp).as_object();
		if (root.contains("valid") && root["valid"].as_bool())
		{
			ketqua_key result{ true, "" };
			if (root.contains("ngayhethan"))
				result.ngayhethan = root["ngayhethan"].as_string().c_str();
			return result;
		}
	} catch (...)
	{
		td_log(loai_log::loi, "Lỗi parse JSON key");
	}
	return std::nullopt;
}
