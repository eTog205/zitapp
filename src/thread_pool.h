//thread_pool.h
#pragma once
#include <boost/asio.hpp>

class ThreadPool
{
public:
	// Khởi tạo thread pool với số lượng thread được chỉ định
	explicit ThreadPool(std::size_t sl_luồng);

	// Xóa bỏ copy constructor và copy assignment
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	// Cho phép move constructor và move assignment (nếu muốn)
	ThreadPool(ThreadPool&&) noexcept = delete;
	ThreadPool& operator=(ThreadPool&&) noexcept = delete;

	// Hủy bỏ và join các thread khi hủy đối tượng
	~ThreadPool();

	// Hàm enqueue cho phép nạp (submit) các tác vụ với hàm trả về std::future
	template<typename Func, typename... Args>
	auto enqueue(Func&& f, Args&&... args)
		-> std::future<std::invoke_result_t<Func, Args...>>;

	// Hàm join đợi cho đến khi các tác vụ trong pool hoàn thành
	void join();

private:
	boost::asio::thread_pool pool;
};

template<typename Func, typename... Args>
auto ThreadPool::enqueue(Func&& f, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>
{
	using return_type = std::invoke_result_t<Func, Args...>;
	// Tạo một packaged_task để bọc hàm và các đối số
	auto task = std::make_shared<std::packaged_task<return_type()>>(
		std::bind(std::forward<Func>(f), std::forward<Args>(args)...)
	);
	std::future<return_type> res = task->get_future();

	boost::asio::post(pool, [task]()
	{
		(*task)();
	});
	return res;
}

extern ThreadPool pool;
