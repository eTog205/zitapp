//thread_pool.cpp
#include "thread_pool.h"

ThreadPool pool(2);

// Khởi tạo thread pool với số lượng thread xác định
ThreadPool::ThreadPool(const std::size_t sl_luồng)
	: pool(sl_luồng)
{
}

// Hủy đối tượng thread pool sẽ join (chờ) tất cả các thread hoàn thành
ThreadPool::~ThreadPool()
{
	join();
}

// Hàm join đợi cho đến khi các tác vụ trong pool hoàn thành
void ThreadPool::join()
{
	pool.join();
}


