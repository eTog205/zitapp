//get.h
#pragma once

#include <string>


bool download_latest_release();

bool download_file(const std::string& url, const std::string& save_path);


