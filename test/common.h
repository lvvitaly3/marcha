#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <memory>
#include <iostream>
#include <stdint.h>
#include <condition_variable>
#include <windows.h>

#define _MAXLEN_PACKET_DATA_  1024
#define _MAXNUM_THREADS_      2
extern std::condition_variable g_cv;

typedef struct TestDatasMes {
	uint32_t idx = 0;
	uint8_t* pdata = NULL;
	int lendata = 0;
} TestDatasMes;
