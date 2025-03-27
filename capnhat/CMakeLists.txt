# E:/source/zitapp/capnhat/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# include thêm thư mục include/
include_directories(include)

add_executable(capnhat cn_main.cpp
    capnhat.cpp
    get.cpp
    giainen.cpp
)

target_sources(capnhat PRIVATE ${CMAKE_SOURCE_DIR}/src/log_nhalam.cpp)

target_link_libraries(capnhat PRIVATE 
    Boost::json
    Boost::asio
    Boost::beast
    Boost::filesystem
    unofficial::sqlite3::sqlite3
    spdlog::spdlog
    OpenSSL::SSL
    OpenSSL::Crypto
)