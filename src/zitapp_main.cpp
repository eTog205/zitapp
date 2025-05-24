// CL_PhanMem.cpp
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

#include <boost/interprocess/sync/named_mutex.hpp>
#include "app.h"
#include "chay_luongphu.h"
#include "kiemtra_ketnoi_mang.h"
#include "log_nhalam.h"

constexpr const char* ten_mutex = "zitapp";
constexpr const char* ten_shm = "zitapp_shm";

namespace bi = boost::interprocess;

namespace
{
	void giaiphong_khoa()
	{
		bi::named_mutex::remove(ten_mutex);
		bi::shared_memory_object::remove(ten_shm);
	}

	BOOL WINAPI xu_ly_tin_hieu(DWORD type)
	{
		giaiphong_khoa();
		return FALSE;
	}

	bool tien_trinh_con_song(DWORD pid)
	{
		HANDLE h = OpenProcess(SYNCHRONIZE, FALSE, pid);
		if (!h)
			return false;

		bool alive = (WaitForSingleObject(h, 0) == WAIT_TIMEOUT);
		CloseHandle(h);
		return alive;
	}
} // namespace

int main()
{
	SetConsoleCtrlHandler(xu_ly_tin_hieu, TRUE);
	atexit(giaiphong_khoa); // log nếu phát hiện xảy ra lỗi

	try
	{
		bi::shared_memory_object shm(bi::open_only, ten_shm, bi::read_only);
		bi::mapped_region region(shm, bi::read_only);
		DWORD pid = *static_cast<DWORD*>(region.get_address());

		if (tien_trinh_con_song(pid))
		{
			MessageBoxA(nullptr, "Phần mềm đã chạy, không thể mở thêm!", "Cảnh báo", MB_OK | MB_ICONWARNING);
			return 1;
		}

		giaiphong_khoa();
	} catch (...)
	{}

	static bi::named_mutex mutex(bi::open_or_create, ten_mutex);
	if (!mutex.try_lock())
	{
		MessageBoxA(nullptr, "Phần mềm đã chạy, không thể mở thêm!", "Cảnh báo", MB_OK | MB_ICONWARNING);
		return 1;
	}

	bi::shared_memory_object shm(bi::create_only, ten_shm, bi::read_write);
	shm.truncate(sizeof(DWORD));
	bi::mapped_region region(shm, bi::read_write);
	*static_cast<DWORD*>(region.get_address()) = GetCurrentProcessId();

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
}
