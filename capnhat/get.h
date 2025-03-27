//get.h
#pragma once

#include <boost/json.hpp>
#include <string>

namespace json = boost::json;

struct duan
{
	std::string sohuu = "eTog205";
	std::string kho = "ZAppCoBan";
};

extern std::string tentep;

std::string send_http_request(const std::string& url, bool is_asset = false, int redirect_count = 0);

json::value parse_json_response(const std::string& response_body);

std::string get_release_tag();

bool download_latest_release();

bool download_file(const std::string& url, const std::string& save_path);


