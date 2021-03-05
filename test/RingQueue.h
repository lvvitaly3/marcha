#pragma once

#include "common.h"

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
	void put(TestDatasMes* pd) {
		std::lock_guard<std::mutex> lock(mtx_);
		vring.push_back(pd);
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

class RingQueues
{
public:
	static void Create() {
		for (unsigned i=0; i<_MAXNUM_THREADS_; i++) {
			std::shared_ptr<RingQueue>ptr(new RingQueue);
			RingQueue_[i] = ptr;
		}
	}
	static void put(TestDatasMes* pd, int ind) {
		RingQueue_[ind]->put(pd);
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




