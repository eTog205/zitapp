# E:/source/zitapp/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

set(CMAKE_COLOR_DIAGNOSTICS ON)

# policy boost mới
cmake_policy(SET CMP0167 NEW)

project(zitapp LANGUAGES CXX)
add_compile_definitions(_WIN32_WINNT=0x0A00)

# tự động chạy vcpkg install
if(EXISTS "E:/dev/vcpkg/vcpkg.exe")
  message(STATUS "[vcpkg] ✅ Đã tìm thấy vcpkg, đang cài gói...")
  execute_process(
    COMMAND "E:/dev/vcpkg/vcpkg.exe" install
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE _vcpkg_result
  )
  if(NOT _vcpkg_result EQUAL 0)
    message(FATAL_ERROR "[vcpkg] ❌ vcpkg install thất bại với mã ${_vcpkg_result}")
  else()
    message(STATUS "[vcpkg] ✅ Cài đặt gói thành công.")
  endif()
else()
  message(WARNING "[vcpkg] ⚠ Không tìm thấy vcpkg ở E:/dev/vcpkg/vcpkg.exe → bỏ qua cài đặt.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include thêm thư mục include/
include_directories(${CMAKE_SOURCE_DIR} include)

# gộp source
file(GLOB_RECURSE ZITAPP_SOURCES CONFIGURE_DEPENDS src/*.cpp)

# copy toàn bộ thư mục tài nguyên vào thư mục build
file(COPY ${CMAKE_SOURCE_DIR}/tainguyen DESTINATION ${CMAKE_BINARY_DIR})

# tạo executable
add_executable(zitapp ${ZITAPP_SOURCES})

target_sources(zitapp PRIVATE zitapp.rc)

# Tìm các thư viện
find_package(Boost REQUIRED COMPONENTS json asio beast process unordered)
find_package(boost_dll CONFIG REQUIRED)
find_package(boost_interprocess CONFIG REQUIRED)
find_package(boost_property_tree CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

add_subdirectory(capnhat EXCLUDE_FROM_ALL)

# Gán link thư viện vào target chính
target_link_libraries(zitapp PRIVATE
    imgui::imgui
    glfw
    Boost::json
    Boost::asio
    Boost::beast
    Boost::unordered
    Boost::process
    Boost::dll
    Boost::interprocess
    Boost::property_tree
    OpenSSL::SSL
    OpenSSL::Crypto
    unofficial::sqlite3::sqlite3
    spdlog::spdlog
    opengl32
)