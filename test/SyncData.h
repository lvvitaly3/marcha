#pragma once

#include "RingQueue.h"

class SyncData
{
public:
	SyncData() {
		idx = -1;
		is_exit = false;
		run();
	}
	virtual ~SyncData() {
		is_exit = true;
		g_cv.notify_all();
		thr->join();
	}
private:
	void run() {
		thr = std::make_shared<std::thread>(&SyncData::fncthr, this);
	}
	void fncthr() {
		while (!is_exit) {
			uint32_t min_idx;
			int nthr = -1;
			// получение мин значения индекса поступающих из очереди пакетов
			for (unsigned i = 0; i < _MAXNUM_THREADS_; i++) {
				TestDatasMes* pd = RingQueues::get(i);
				if (pd) {
					if (idx == -1) {
						if (i == 0) {
							min_idx = pd->idx;
							nthr = i;
						}
						else {
							if (min_idx > pd->idx) {
								min_idx = pd->idx;
								nthr = i;
							}
						}
					}
					else {
						if ((idx & 0x0ffffffff) + 1 == pd->idx) {
							nthr = i;
							min_idx = pd->idx;
							break;
						}
						else 
						if ((idx & 0x0ffffffff) + 1 > pd->idx) {
							RingQueues::del(i);
						}
					}
				}
			}//EndFor
			if (nthr > -1) {
			    idx = min_idx;
//				printf("------------------->%u\n", (unsigned)idx);
		    }
			else {
//				std::unique_lock<std::mutex> lk(mtx_);
//				g_cv.wait(lk);
//				Sleep(1);
			}
		}
		printf("------------------->%u\n", (unsigned)idx);

	}
	std::mutex mtx_;
	uint64_t idx;
	bool is_exit;
	std::shared_ptr<std::thread> thr;

};

