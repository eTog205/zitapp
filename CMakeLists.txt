cmake_minimum_required(VERSION 3.30)

project(zitapp LANGUAGES CXX)

add_compile_options("/FI${CMAKE_SOURCE_DIR}/include/preboost_winsock.hpp")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0167 NEW)

add_compile_definitions(_WIN32_WINNT=0x0A00 BOOST_ALL_NO_DEPRECATED)

if(NOT DEFINED ENV{VCPKG_ROOT})
  message(FATAL_ERROR "[vcpkg] ❌ VCPKG_ROOT chưa được thiết lập.")
endif()

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(boost_json CONFIG REQUIRED)
find_package(boost_asio CONFIG REQUIRED)
find_package(boost_beast CONFIG REQUIRED)
find_package(boost_unordered CONFIG REQUIRED)
find_package(boost_process CONFIG REQUIRED)
find_package(boost_dll CONFIG REQUIRED)
find_package(boost_interprocess CONFIG REQUIRED)
find_package(boost_property_tree CONFIG REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tienich
    ${CMAKE_SOURCE_DIR}/giaodien
    ${CMAKE_SOURCE_DIR}/csdl
)

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

target_link_libraries(chung PUBLIC
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

add_executable(zitapp ${ZITAPP_SOURCES})

target_link_libraries(zitapp PRIVATE chung)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/zitapp.rc")
    target_sources(zitapp PRIVATE zitapp.rc)
endif()

file(COPY ${CMAKE_SOURCE_DIR}/tainguyen DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(capnhat)