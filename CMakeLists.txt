# E:/source/zitapp/CMakeLists.txt
cmake_minimum_required(VERSION 3.30)
cmake_policy(SET CMP0167 NEW)

project(zitapp LANGUAGES CXX)

set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

find_package(Boost REQUIRED COMPONENTS json asio beast process unordered dll interprocess property_tree)
find_package(spdlog REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE ZITAPP_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE TIENICH_SRC CONFIGURE_DEPENDS "tienich/*.cpp" "tienich/*.h")
file(GLOB_RECURSE GIAODIEN_SRC CONFIGURE_DEPENDS "giaodien/*.cpp" "giaodien/*.h")
file(GLOB_RECURSE CSDL_SRC CONFIGURE_DEPENDS "csdl/*.cpp" "csdl/*.h")

add_library(chung STATIC
    ${TIENICH_SRC}
    ${GIAODIEN_SRC}
    ${CSDL_SRC}
)

target_include_directories(chung PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tienich
    ${CMAKE_SOURCE_DIR}/giaodien
    ${CMAKE_SOURCE_DIR}/csdl
)

add_executable(zitapp ${ZITAPP_SOURCES})

target_include_directories(zitapp PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tienich
    ${CMAKE_SOURCE_DIR}/giaodien
    ${CMAKE_SOURCE_DIR}/csdl
)

target_link_libraries(zitapp PRIVATE
    chung
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

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/zitapp.rc")
    target_sources(zitapp PRIVATE zitapp.rc)
endif()

# copy toàn bộ thư mục tài nguyên vào thư mục build
file(COPY ${CMAKE_SOURCE_DIR}/tainguyen DESTINATION ${CMAKE_BINARY_DIR})
add_subdirectory(capnhat EXCLUDE_FROM_ALL)