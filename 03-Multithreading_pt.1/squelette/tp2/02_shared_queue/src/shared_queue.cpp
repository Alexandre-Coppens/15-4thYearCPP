#include "shared_queue.hpp"

void SharedQueue::push(int id) {
  std::lock_guard<std::mutex> lock(mutex_);
  tasks_.push(id);
}

bool SharedQueue::try_pop(int& id) {
	if (tasks_.empty())
	{
		return false;
	}

	mutex_.lock();
	id = tasks_.front();
	tasks_.pop();
	mutex_.unlock();

	return true;
}
