// CL_PhanMem.cpp
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#include <boost/interprocess/sync/named_mutex.hpp>
#include "app.h"
#include "chay_luongphu.h"
#include "kiemtra_ketnoi_mang.h"
#include "log_nhalam.h"

int main()
{
	const auto ten_tientrinh = "ZitApp";
	static boost::interprocess::named_mutex mutex(boost::interprocess::open_or_create, ten_tientrinh);

	if (!mutex.try_lock())
	{
		MessageBoxA(nullptr, "Phần mềm đã chạy, không thể mở thêm!", "Cảnh báo", MB_OK | MB_ICONWARNING);
		return 1;
	}

	constexpr int mode = 1;
	g_logger = chuyendoi(mode);

	if (kiemtra_trangthai_mang())
	{
		auto f = lp_chay_capnhat();
		f.wait();
	}

	GLFWwindow* cuaso = khoitao_cuaso();
	if (!cuaso)
		return 1;

#ifdef _WIN32
	dat_taskbar_icon(cuaso);
#endif

	vonglap_chinh(cuaso);

	dondep(cuaso);

	if (mode == 1)
		flush_memory_logs_to_file();

	spdlog::shutdown();

	boost::interprocess::named_mutex::remove(ten_tientrinh);
}
