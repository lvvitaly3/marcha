#pragma once

#include "common.h"
#include "RingQueue.h"
extern uint32_t g_idx;

class PrepareData
{
public:
	PrepareData() {
		is_exit = false;
		run();
	}
	virtual ~PrepareData() {
		is_exit = true;
		for (int i = 0; i < _MAXNUM_THREADS_; i++) {
			vthr[i]->join();
		}
	}
	void SetExit() {
		is_exit = true;
	}
private:
	std::mutex mtx_;
	std::vector<std::shared_ptr<std::thread>> vthr;
	volatile bool is_exit;
	void run() {
		for (int i = 0; i < _MAXNUM_THREADS_; i++) {
			std::shared_ptr<std::thread> ptr(new std::thread(&PrepareData::fncThread, this, i));
			vthr.push_back(ptr);
		}
	}
	uint32_t getIdx() {
		std::lock_guard<std::mutex> lock(mtx_);
		return g_idx++;
	}
	void fncThread(int32_t pos) {
		while (!is_exit) {
			TestDatasMes* ptmes = new TestDatasMes;
			ptmes->idx = getIdx();
			RingQueues::put(ptmes, pos);
			g_cv.notify_one();
			Sleep(1);
		}
	}
};

