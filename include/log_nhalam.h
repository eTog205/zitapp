//log_nhalam.h
#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

// Kiểu enum lưu trữ loại log
enum class loai_log :uint8_t
{
	thong_bao,
	canh_bao,
	loi
};

// Lấy thời gian hiện tại theo định dạng "YYYY-MM-DD_HH-MM-SS"
std::string lay_dinhdang_tg_hientai1();

// Hàm quay vòng file log
void quayvong_log(const fs::path& thumuc_log, const fs::path& log_hientai);

// Macro ghi log
#define td_log(loai, thongdiep) \
    do { \
        const auto tentep = fs::path(__FILE__).filename().string(); \
        if ((loai) == loai_log::thong_bao) { \
            g_logger->info("[✅Thông báo] ({}): {}", tentep, thongdiep); \
        } else if ((loai) == loai_log::canh_bao) { \
            g_logger->warn("[⚠️Cảnh báo] ({}): {}", tentep, thongdiep); \
        } else if ((loai) == loai_log::loi) { \
            g_logger->error("[❌Lỗi] ({}): {}", tentep, thongdiep); \
        } \
    } while(0)

// Đối tượng logger toàn cục dùng trong toàn dự án
extern std::shared_ptr<spdlog::logger> g_logger;

// Hàm chuyển đổi chế độ log:
// Nếu mode == 1 => sử dụng chế độ mới (memory sink)
// Nếu mode == 0 => sử dụng chế độ cũ (file sink)
std::shared_ptr<spdlog::logger> chuyendoi(int mode);

// Nếu dùng chế độ mới, dùng hàm này để ghi log từ bộ nhớ ra file trước shutdown.
void flush_memory_logs_to_file();


