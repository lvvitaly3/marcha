#include "sync.h"

uint32_t main_idx = 0;
uint32_t calc_idx = -1;
std::condition_variable sync_cv;
