#include "net.h"
#include "log_nhalam.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>

#include <boost/beast/version.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace bj = boost::json;
namespace ssl = net::ssl;
namespace json = boost::json;

namespace
{
	http::request<http::string_body> tao_yeucau_HTTP(const std::string& host, const std::string& target, const std::string& user_agent, const std::string& accept, const std::string& port)
	{
		http::request<http::string_body> req{ http::verb::get, target, 11 };
		req.set(http::field::host, host);
		req.set(http::field::user_agent, user_agent);
		req.set(http::field::accept, accept);
		return req;
	}
}

std::string http_post(const std::string& noidung_json)
{
	cauhinh_api ap;
	try
	{
		net::io_context ioc;
		net::ssl::context ctx(net::ssl::context::tlsv12_client);
		ctx.set_default_verify_paths();

		beast::ssl_stream<tcp::socket> stream(ioc, ctx);
		tcp::resolver resolver(ioc);

		auto const results = resolver.resolve(ap.host, ap.port);
		net::connect(stream.next_layer(), results.begin(), results.end());
		stream.handshake(net::ssl::stream_base::client);

		http::request<http::string_body> req = tao_yeucau_HTTP(ap.host, ap.target, "ZitApp/1.0", ap.accept, ap.port);

		req.body() = noidung_json;
		req.prepare_payload();

		http::write(stream, req);

		beast::flat_buffer buffer;
		http::response<http::string_body> res;

		read(stream, buffer, res);
		stream.shutdown();

		return res.body();
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "HTTP POST lỗi: " + std::string(e.what()));
		return "";
	}
}

std::string gui_yeucau_http(const std::string& target)
{
	cauhinh_github gh;
	try
	{
		net::io_context ioc;
		boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
		ctx.set_default_verify_paths();

		tcp::resolver resolver(ioc);
		beast::ssl_stream<tcp::socket> stream(ioc, ctx);

		auto const results = resolver.resolve(gh.host, gh.port);
		net::connect(stream.next_layer(), results.begin(), results.end());
		stream.handshake(boost::asio::ssl::stream_base::client);

		http::request<http::string_body> req = tao_yeucau_HTTP(gh.host, target, "Boost.Beast", gh.accept_raw, gh.port);

		http::write(stream, req);

		// Nhận phản hồi
		beast::flat_buffer buffer;
		http::response_parser<http::dynamic_body> parser;
		parser.body_limit(std::numeric_limits<std::uint64_t>::max());
		read(stream, buffer, parser);
		auto& res = parser.get();

		// Đóng kết nối
		beast::error_code ec;
		auto shutdown_ec = stream.shutdown(ec); // không bỏ shutdown_ec

		if (ec == net::error::eof)
			ec = {};

		if (ec)
			throw beast::system_error(ec);

		if (res.result() != http::status::ok)
			throw std::runtime_error("HTTP Error: " + std::to_string(static_cast<int>(res.result())));

		return buffers_to_string(res.body().data());

	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "HTTP Request: " + std::string(e.what()));
		return "";
	}
}

std::string gui_yeucau_http2(const std::string& url, bool is_asset, int redirect_count)
{
	if (redirect_count > 5)
	{
		td_log(loai_log::loi, "Quá số lần redirect cho phép.");
		return "";
	}

	try
	{
		const std::string https_prefix = "https://";
		if (!url.starts_with(https_prefix))
		{
			td_log(loai_log::loi, "URL không hợp lệ: " + url);
			return "";
		}

		std::string url_without_https = url.substr(https_prefix.size());
		auto pos = url_without_https.find('/');
		if (pos == std::string::npos)
		{
			td_log(loai_log::loi, "URL không có target hợp lệ: " + url);
			return "";
		}

		std::string host = url_without_https.substr(0, pos);
		std::string target = url_without_https.substr(pos);

		net::io_context ioc;
		ssl::context ctx{ ssl::context::tlsv12_client };
		ctx.set_default_verify_paths();

		beast::ssl_stream<beast::tcp_stream> stream{ ioc, ctx };

		tcp::resolver resolver{ ioc };
		auto const results = resolver.resolve(host, "443");
		get_lowest_layer(stream).connect(results);

		if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
		{
			beast::error_code ec{ static_cast<int>(::ERR_get_error()), net::error::get_ssl_category() };
			td_log(loai_log::loi, "SNI error: " + ec.message());
			throw beast::system_error{ ec };
		}

		stream.handshake(ssl::stream_base::client);

		http::request<http::empty_body> req{ http::verb::get, target, 11 };
		req.set(http::field::host, host);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		if (is_asset)
			req.set(http::field::accept, "application/octet-stream");

		http::write(stream, req);
		
		beast::flat_buffer buffer;
		http::response_parser<http::dynamic_body> parser;
		parser.body_limit(std::numeric_limits<std::uint64_t>::max());
		read(stream, buffer, parser);
		auto& res = parser.get();

		if (res.result() == http::status::moved_permanently || res.result() == http::status::found || res.result() == http::status::temporary_redirect ||
			res.result() == http::status::permanent_redirect)
		{
			auto loc = res.find(http::field::location);
			if (loc != res.end())
			{
				auto new_url = std::string(loc->value().data(), loc->value().size());
				return gui_yeucau_http2(new_url, is_asset, redirect_count + 1);
			}
			td_log(loai_log::loi, "Không tìm thấy trường 'Location' trong phản hồi redirect.");
			return "";
		}

		if (res.result() != http::status::ok)
		{
			td_log(loai_log::loi, "[send_http_request] HTTP lỗi: " + std::to_string(static_cast<int>(res.result())));
			return "";
		}

		beast::error_code ec;

		stream.next_layer().socket().shutdown(tcp::socket::shutdown_both, ec);
		if (ec)
		{
			td_log(loai_log::loi, "[send_http_request] Lỗi khi shutdown socket: " + ec.message());
		}

		stream.next_layer().socket().close(ec);
		if (ec)
		{
			td_log(loai_log::loi, "[send_http_request] Lỗi khi đóng socket: " + ec.message());
		}

		return buffers_to_string(res.body().data());
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "[send_http_request] Ngoại lệ: " + std::string(e.what()));
		return "";
	}
}


std::string lay_thongtin_github(const std::string& file_path)
{
	cauhinh_github gh;
	const std::string target = "/repos/" + gh.owner + "/" + gh.repo_tainguyen + "/contents/" + file_path;

	std::string response = gui_yeucau_http(target);
	if (response.empty())
	{
		td_log(loai_log::loi, "Không thể tải dữ liệu từ GitHub");
		return "";
	}

	return response;
}

std::string lay_thongtin_tep_github(const std::string& file_path)
{
	cauhinh_github gh;
	const std::string target = "/repos/" + gh.owner + "/" + gh.repo_tainguyen + "/commits?path=" + file_path;

	std::string response = gui_yeucau_http(target);

	if (response.empty())
	{
		td_log(loai_log::loi, "Không thể lấy metadata của file từ GitHub");
		return "";
	}

	return response;
}

std::string get_release_tag()
{
	const std::string url = "https://api.github.com/repos/eTog205/zitapp/releases/latest";
	const std::string response_body = gui_yeucau_http2(url, false);
	if (response_body.empty())
	{
		td_log(loai_log::loi, "[get_release_tag] Không nhận được phản hồi từ GitHub API!");
		return "";
	}

	json::value jv = json::parse(response_body);
	if (!jv.is_object() || !jv.as_object().contains("tag_name"))
	{
		td_log(loai_log::canh_bao, "Không tìm thấy 'tag_name' trong phản hồi JSON.");
		return "";
	}

	auto tag_name = json::value_to<std::string>(jv.as_object()["tag_name"]);
	td_log(loai_log::thong_bao, "Lấy phiên bản mới nhất thành công: " + tag_name);
	return tag_name;
}

std::optional<ketqua_key> kiemtra_key_online(const std::string& key)
{
	bj::object body_obj;
	body_obj["key"] = key;

	std::string res = http_post(serialize(body_obj));
	if (res.empty())
		return std::nullopt;

	try
	{
		auto obj = bj::parse(res).as_object();
		if (obj.contains("valid") && obj["valid"].as_bool())
		{
			ketqua_key kq;
			kq.hop_le = true;
			if (obj.contains("ngayhethan"))
				kq.ngayhethan = obj["ngayhethan"].as_string().c_str();
			return kq;
		}
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "Lỗi parse JSON key: " + std::string(e.what()));
	}
	return std::nullopt;
}
