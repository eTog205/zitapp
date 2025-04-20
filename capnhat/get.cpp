// get.cpp
#include "get.h"
#include "log_nhalam.h"
#include "net.h"

#include <boost/json.hpp>
#include <fstream>

namespace json = boost::json;

std::string tentep;

bool download_file(const std::string& url, const std::string& save_path)
{
	const std::string file_content = tai_url_http(url, true);

	if (file_content.empty())
	{
		td_log(loai_log::loi, "[download_file] Nội dung tải về rỗng.");
		return false;
	}

	std::ofstream ofs(save_path, std::ios::binary);
	if (!ofs)
	{
		td_log(loai_log::loi, "[download_file] Không mở được file để ghi: " + save_path);
		return false;
	}
	ofs.write(file_content.data(), static_cast<std::streamsize>(file_content.size()));
	ofs.close();

	return true;
}

bool download_latest_release()
{
	cauhinh_github gh;

	const std::string target = "/repos/" + gh.owner + "/" + gh.repo_app + "/releases/latest";

	const std::string response_body = tai_url_http("https://" + gh.host + target);
	if (response_body.empty())
	{
		td_log(loai_log::loi, "[download_latest_release] Không nhận được phản hồi từ GitHub API!");
		return false;
	}

	json::value jv = json::parse(response_body);
	if (!jv.is_object() || !jv.as_object().contains("assets"))
	{
		td_log(loai_log::canh_bao, "Không tìm thấy 'assets' trong phản hồi JSON.");
		return false;
	}

	auto& assets = jv.as_object()["assets"].as_array();
	if (assets.empty())
	{
		td_log(loai_log::canh_bao, "Bản phát hành mới nhất không chứa asset nào.");
		return false;
	}

	auto& asset = assets[0].as_object();
	if (!asset.contains("browser_download_url"))
	{
		td_log(loai_log::canh_bao, "Asset không chứa 'browser_download_url'.");
		return false;
	}

	const auto download_url = json::value_to<std::string>(asset["browser_download_url"]);

	if (asset.contains("name"))
	{
		tentep = json::value_to<std::string>(asset["name"]);
	}
	else // cần làm: xem lại có cần thiết không
	{
		tentep = "AppZit.rar";
	}

	return download_file(download_url, tentep);
}
