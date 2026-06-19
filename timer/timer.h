#ifndef TIMER_H
#define TIMER_H

typedef struct {
  double startSolveTime;
  double startInspectionTime;
  double elapsedInspectionTime;

  int running;
  int inspectionActive;
} SolveTimer;

#endif