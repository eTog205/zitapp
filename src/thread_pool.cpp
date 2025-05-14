// thread_pool.cpp
#include "thread_pool.h"

ThreadPool pool(2);

ThreadPool::ThreadPool(const std::size_t sl_luồng) : pool(sl_luồng)
{}

ThreadPool::~ThreadPool()
{
	join();
}

void ThreadPool::join()
{
	pool.join();
}
