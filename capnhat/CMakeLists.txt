# E:/source/zitapp/capnhat/CMakeLists.txt
cmake_minimum_required(VERSION 3.30)

project(capnhat LANGUAGES CXX)

file(GLOB_RECURSE CAPNHAT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(capnhat ${CAPNHAT_SOURCES})

target_include_directories(capnhat PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(chung PUBLIC spdlog::spdlog)
target_link_libraries(capnhat PRIVATE
    chung
    Boost::json
    Boost::asio
    Boost::beast
    Boost::filesystem
    OpenSSL::SSL
    OpenSSL::Crypto
    unofficial::sqlite3::sqlite3
    spdlog::spdlog
)