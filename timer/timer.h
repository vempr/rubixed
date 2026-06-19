#ifndef TIMER_H
#define TIMER_H

typedef struct {
  double startSolveTime;
  double startInspectionTime;

  int running;
  int inspectionActive;
} SolveTimer;

double get_time_elapsed(double start);

#endif