#ifndef TIMER_H
#define TIMER_H

typedef struct {
  double startSolveTime;
  double startInspectionTime;
  double startHoldTime;

  int running;
  int inspectionActive;
  int ready;

  int spaceHeld;
  int armed;
  int dnf;
} SolveTimer;

double get_time_elapsed(double start);

#endif