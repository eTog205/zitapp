#include "log_nhalam.h"
#include "net.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace bj = boost::json;

std::string http_get(const cauhinh_chung& ch)
{
	try
	{
		net::io_context ioc;
		net::ssl::context ctx(net::ssl::context::tlsv12_client);
		ctx.set_default_verify_paths();

		beast::ssl_stream<tcp::socket> stream(ioc, ctx);
		tcp::resolver resolver(ioc);

		auto const results = resolver.resolve(ch.host, ch.port);
		net::connect(stream.next_layer(), results.begin(), results.end());
		stream.handshake(net::ssl::stream_base::client);

		http::request<http::string_body> req{ http::verb::get, ch.target, 11 };
		req.set(http::field::host, ch.host);
		req.set(http::field::user_agent, "ZitApp/1.0");
		req.set(http::field::accept, ch.accept);

		http::write(stream, req);

		beast::flat_buffer buffer;
		http::response_parser<http::dynamic_body> parser;

		parser.body_limit(ch.gioihan_body);
		read(stream, buffer, parser);
		stream.shutdown();

		auto& res = parser.get();
		return buffers_to_string(res.body().data());
	} catch (const std::exception& e)
	{
		td_log(loai_log::loi, "HTTP GET lỗi: " + std::string(e.what()));
		return "";
	}
}

std::string http_post(const cauhinh_chung& ch, const std::string& noidung_json)
{
	try
	{
		net::io_context ioc;
		net::ssl::context ctx(net::ssl::context::tlsv12_client);
		ctx.set_default_verify_paths();

		beast::ssl_stream<tcp::socket> stream(ioc, ctx);
		tcp::resolver resolver(ioc);

		auto const results = resolver.resolve(ch.host, ch.port);
		net::connect(stream.next_layer(), results.begin(), results.end());
		stream.handshake(net::ssl::stream_base::client);

		http::request<http::string_body> req{ http::verb::post, ch.target, 11 };
		req.set(http::field::host, ch.host);
		req.set(http::field::user_agent, "ZitApp/1.0");
		req.set(http::field::content_type, ch.accept);

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

std::string tai_file_github(const cauhinh_github& gh, const std::string& filepath)
{
	cauhinh_chung ch;
	ch.host = gh.host;
	ch.target = "/repos/" + gh.owner + "/" + gh.repo + "/contents/" + filepath;
	ch.accept = gh.accept_raw;
	return http_get(ch);
}

std::string lay_sha_github(const cauhinh_github& gh, const std::string& filepath)
{
	cauhinh_chung ch;
	ch.host = gh.host;
	ch.target = "/repos/" + gh.owner + "/" + gh.repo + "/commits?path=" + filepath;
	ch.accept = gh.accept_json;
	return http_get(ch);
}

std::optional<ketqua_key> kiemtra_key_online(const cauhinh_api& ap, const std::string& key)
{
	bj::object body_obj;
	body_obj["key"] = key;

	cauhinh_chung ch;
	ch.host = ap.host;
	ch.port = ap.port;
	ch.target = ap.target;

	std::string res = http_post(ch, bj::serialize(body_obj));
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
