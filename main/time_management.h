#ifndef TIME_MANAGEMENT_H
#define TIME_MANAGEMENT_H

#include "common.h"

void setup_time_management();
struct tm read_time();
unsigned long long read_epoch_time_in_msec();
void loop_time();

#endif
