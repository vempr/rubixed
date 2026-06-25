#ifndef LOG_H
#define LOG_H

#define SOLVES_FILE "./storage/solves.csv"
#define MAX_SOLVES 65536

typedef struct {
  char *timestamp;
  char *scramble;
  double time;
  int plus2;
  int dnf;
  char *mode;
} SolveEntry;

void log_solve(const char *scramble, double time, int plus2, int dnf, const char *mode);

#endif