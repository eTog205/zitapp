#include "kiemtra_ketnoi_mang.h"

#include <wininet.h>

#pragma comment(lib, "wininet.lib")

bool kiemtra_trangthai_mang()
{
	HINTERNET net = InternetOpenW(L"Internet Checker", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
	if (net == nullptr)
		return false;

	HINTERNET conn = InternetOpenUrlW(net, L"http://www.google.com", nullptr, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
	bool ketnoi_ok = conn != nullptr;

	if (conn)
		InternetCloseHandle(conn);
	InternetCloseHandle(net);

	return ketnoi_ok;
}
