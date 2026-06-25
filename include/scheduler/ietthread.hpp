#pragma once
#include <atomic>
class IETThread {
public: 
	IETThread() = default;
	~IETThread() = default;

	void start();
	void stop();
	static void sleep(int ms);
	bool finished() const;
protected:
	virtual void run() = 0;
	bool isRunning() const;

private:
	std::atomic<bool> running{ false };
	std::atomic<bool> hasStopped{ true };
	
};