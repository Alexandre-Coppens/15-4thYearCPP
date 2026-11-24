#include "task_queue.hpp"

void TaskQueue::push(std::function<void()> task) {
  // TODO: sous lock_guard, pousser la tâche.
  // TODO: notify_one (idéalement après avoir libéré le mutex).
	mutex_.lock();
	tasks_.push(task);
	mutex_.unlock();
	available_.notify_one();
}

bool TaskQueue::pop(std::function<void()>& task) {
  // TODO: unique_lock + available_.wait(lock, prédicat stop_ || !tasks_.empty()).
  // TODO: si stop_ && tasks_.empty(), retourner false.
  // TODO: sinon déplacer la tête dans task, pop, retourner true.

	std::unique_lock lock(mutex_);
	available_.wait(lock, [this]() { return stop_ || !tasks_.empty(); });

	if (stop_ || tasks_.empty()) return false;

	task = tasks_.front();
	tasks_.pop();

	return true;
}

void TaskQueue::request_stop() {
  // TODO: sous mutex, stop_ = true ; puis notify_all.
	{
		std::lock_guard<std::mutex> lock(mutex_);
		stop_ = true;
	}
	available_.notify_all();
}
