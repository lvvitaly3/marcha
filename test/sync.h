#pragma once

#include "common.h"

extern uint32_t main_idx;
extern uint32_t calc_idx;
extern std::condition_variable sync_cv;

class sync
{
public:
	sync() {
		is_exit = false;
		run();
	}
	virtual ~sync() {
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
			std::shared_ptr<std::thread> ptr(new std::thread(&sync::fncThread, this, i));
			vthr.push_back(ptr);
		}
	}
	uint32_t getIdx() {
		std::lock_guard<std::mutex> lock(mtx_);
		return main_idx++;
	}
	void fncThread(int32_t pos) {
		while (!is_exit) {
			uint32_t idx = getIdx();
			while (!is_exit) {
				if (idx == 0 && calc_idx == 0xffffffff) {
					calc_idx = idx;
					std::cout << "--------------->" << idx << "\n";
					break;
				}
				else {
					if ((calc_idx & 0x0ffffffff) + 1 == idx) {
						std::cout << "--------------->" << idx << "\n";
						calc_idx = idx;
						break;
					}
					else 
					if ((calc_idx & 0x0ffffffff) + 1 > idx) {
						break;
					}
					else {
						if (_MAXNUM_THREADS_ > 1) {
							sync_cv.notify_one();
							std::unique_lock<std::mutex> lk(mtx_);
							sync_cv.wait_for(lk, std::chrono::milliseconds(1));
						}
						else {
							std::cout << "--------------->" << idx << "\n";
							calc_idx = idx;
							break;
						}
					}
				}
			}
		}
	}
};

