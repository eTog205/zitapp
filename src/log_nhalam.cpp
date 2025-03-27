//log_nhalam.cpp
#include "log_nhalam.h"

#include <fstream>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <windows.h>

using namespace std::chrono;

namespace
{
	template<typename Mutex>
	class memory_sink final : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		std::vector<std::string> buffer;
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			spdlog::memory_buf_t formatted;
			this->formatter_->format(msg, formatted);
			buffer.push_back(fmt::to_string(formatted));
		}
		void flush_() override
		{
			// Không làm gì, sẽ flush khi shutdown
		}
	};

	using memory_sink_mt = memory_sink<std::mutex>;

	// Con trỏ toàn cục dùng cho memory sink khi dùng chế độ mới
	std::shared_ptr<memory_sink_mt> g_memory_sink = nullptr;

	// Hàm khởi tạo logger sử dụng memory sink.
	std::shared_ptr<spdlog::logger> khoitao_logger_memory()
	{
		g_memory_sink = std::make_shared<memory_sink_mt>();
		auto logger = std::make_shared<spdlog::async_logger>(
			"async_logger",
			g_memory_sink,
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block
		);
		logger->set_pattern("%Y-%m-%d %H:%M:%S.%e %v");
		logger->set_level(spdlog::level::trace);
		register_logger(logger);
		return logger;
	}

	// Hàm khởi tạo logger sử dụng file sink.
	std::shared_ptr<spdlog::logger> khoitao_logger_file()
	{
		fs::create_directories("log");
		const fs::path logDir = "log";
		const fs::path currentLog = logDir / "hientai.txt";
		// Quay vòng file log nếu cần.
		quayvong_log(logDir, currentLog);
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(currentLog.string(), false);
		auto logger = std::make_shared<spdlog::async_logger>(
			"async_logger",
			file_sink,
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block
		);
		logger->set_pattern("%Y-%m-%d %H:%M:%S.%e %v");
		logger->set_level(spdlog::level::trace);
		logger->flush_on(spdlog::level::trace);

		register_logger(logger);
		return logger;
	}
}


std::string lay_dinhdang_tg_hientai1()
{
	const auto now = system_clock::now();
	auto in_time_t = system_clock::to_time_t(now);
	std::tm local_tm;
#if defined(_WIN32) || defined(_WIN64)
	(void)localtime_s(&local_tm, &in_time_t);
#else
	localtime_r(&in_time_t, &local_tm);
#endif
	std::stringstream ss;
	ss << std::put_time(&local_tm, "%Y-%m-%d_%H-%M-%S");
	return ss.str();
}

// Hàm quay vòng file log:
// Nếu file hiện tại tồn tại, đổi tên nó thành "<thời gian>.txt"
void quayvong_log(const fs::path& thumuc_log, const fs::path& log_hientai)
{
	if (exists(log_hientai))
	{
		const std::string timestamp = lay_dinhdang_tg_hientai1();
		const fs::path new_file = thumuc_log / (timestamp + ".txt");
		try
		{
			fs::rename(log_hientai, new_file);
		} catch (const fs::filesystem_error& e)
		{
			// Xử lý lỗi nếu cần (ví dụ, ghi log ra một nơi khác hoặc thông báo cho người dùng)
			MessageBoxA(nullptr, e.what(), "Log lỗi: không thể quay vòng", MB_OK | MB_ICONERROR);
		}
	}
}

std::shared_ptr<spdlog::logger> chuyendoi(const int mode)
{
	try
	{
		// Khởi tạo thread pool cho async logger:
		constexpr size_t queue_size = 8192;
		constexpr size_t thread_count = 1;
		spdlog::init_thread_pool(queue_size, thread_count);

		if (mode == 1)
		{
			return khoitao_logger_memory();
		}
		return khoitao_logger_file();
	} catch (const spdlog::spdlog_ex& ex)
	{
		MessageBoxA(nullptr, ex.what(), "Log lỗi: không thể khởi tạo spdlog", MB_OK | MB_ICONERROR);
		return nullptr;
	}
}

std::shared_ptr<spdlog::logger> g_logger = nullptr;

void flush_memory_logs_to_file()
{
	if (g_memory_sink && !g_memory_sink->buffer.empty())
	{
		fs::create_directories("log");
		const fs::path logDir = "log";
		const fs::path currentLog = logDir / "hientai.txt";
		quayvong_log(logDir, currentLog);

		std::ofstream ofs(currentLog.string(), std::ios::out);
		if (ofs.is_open())
		{
			for (const auto& line : g_memory_sink->buffer)
			{
				ofs << line;
			}
			ofs.close();
		}
	}
}


