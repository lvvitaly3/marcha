#pragma once

#include "common.h"

#define _NUMITEMS_QUEUE_  (1024*32)
#define _RING_

#ifdef _RING_

class RingQueue
{
public:
	RingQueue() {
		r_= 0;
		w_= 0;
	}
	virtual ~RingQueue() {
		std::lock_guard<std::mutex> lock(mtx_);
		for (uint64_t i = r_; i < w_; i++) {
			delete(pring_[i %_NUMITEMS_QUEUE_]);
		}
	}
	bool put(TestDatasMes* pd) {
		bool st = false;
		std::lock_guard<std::mutex> lock(mtx_);
		if (w_ - r_ < _NUMITEMS_QUEUE_- 1) {
//			int n = w_ % _NUMITEMS_QUEUE_;
			pring_[w_%_NUMITEMS_QUEUE_] = pd;
			w_++;
			st = true;
		}
		return st;
	}
	TestDatasMes* get(bool f = false) {
		std::lock_guard<std::mutex> lock(mtx_);
		TestDatasMes* pd = NULL;
		if (!empty()) {
//			int n = r_%_NUMITEMS_QUEUE_;
			pd = pring_[r_%_NUMITEMS_QUEUE_];
			if (f) {
				r_++;
			}
		}
		return pd;
	}
	void del() {
		TestDatasMes* pd = get(true);
		if (pd) {
			delete(pd);
		}
	}

private:
	bool empty() {
		if (r_ == w_) {
			return true;
		}
		return false;
	}
private:
	std::mutex mtx_;
	TestDatasMes* pring_[_NUMITEMS_QUEUE_];
	uint64_t r_, w_;
};
#else
class RingQueue
{
public:
	RingQueue() {
		;
	}
	virtual ~RingQueue() {
		std::lock_guard<std::mutex> lock(mtx_);
		for (unsigned i = 0; i < vring.size(); i++) {
			delete(vring[i]);
		}
	}
	bool put(TestDatasMes* pd) {
		bool st = false;
		std::lock_guard<std::mutex> lock(mtx_);
		if (vring.size() < _NUMITEMS_QUEUE_) {
			vring.push_back(pd);
			st = true;
		}
		return st;
	}
	TestDatasMes* get() {
		std::lock_guard<std::mutex> lock(mtx_);
		TestDatasMes* pd = NULL;
		if (!vring.empty()) {
			pd = vring.front();
		}
		return pd;
	}
	void del() {
		std::lock_guard<std::mutex> lock(mtx_);
		if (!vring.empty()) {
			delete(vring.front());
			vring.erase(vring.begin());
		}
	}

private:
	std::mutex mtx_;
	std::vector<TestDatasMes*> vring;
};
#endif

class RingQueues
{
public:
	static void Create() {
		for (unsigned i=0; i<_MAXNUM_THREADS_; i++) {
			std::shared_ptr<RingQueue>ptr(new RingQueue);
			RingQueue_[i] = ptr;
		}
	}
	static bool put(TestDatasMes* pd, int ind) {
		return RingQueue_[ind]->put(pd);
	}
	static TestDatasMes* get(int ind) {
		return RingQueue_[ind]->get();
	}
	static void del(int ind) {
		RingQueue_[ind]->del();
	}

private:
	RingQueues() {}
public:
	static std::shared_ptr<RingQueue> RingQueue_[_MAXNUM_THREADS_];
};



