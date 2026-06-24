#include "scheduler/ietthread.hpp"
#include <thread>
#include <chrono>

void IETThread::start() {
	running.store(true);
	std::thread(&IETThread::run, this).detach();
}

void IETThread::stop() {
	running.store(false);
}

void IETThread::sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}