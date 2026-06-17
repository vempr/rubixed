#ifndef TIMER_H
#define TIMER_H

typedef struct {
  double startSolveTime;
  double elapsedSolveTime;
  double startInspectionTime;
  double elapsedInspectionTime;

  int running;
  int inspectionActive;
} SolveTimer;

#endif