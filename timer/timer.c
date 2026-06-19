#include "timer.h"
#include "../app/app.h"

double get_time_elapsed(double start) {
  return GetTime() - start;
}