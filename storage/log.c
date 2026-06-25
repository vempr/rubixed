#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "log.h"

static void ensure_storage_dir() {
  #ifndef _WIN32
    _mkdir("storage");
  #else
    mkdir("storage", 0755);
  #endif
}

void log_solve(const char *scramble, double solve_time, int plus2, int dnf, const char *mode) {
  ensure_storage_dir();

  printf("logsolve\n");
  FILE *f = fopen(SOLVES_FILE, "a");
  if (!f) {
    f = fopen(SOLVES_FILE, "w");
    if (!f) {
      printf("ERROR: Could not create solves file: %s\n", strerror(errno));
      return;
    }

    // timestamp,scramble,time,plus2,dnf,mode
    fprintf(f, "timestamp,scramble,time,plus2,dnf,mode\n");
    fclose(f);

    f = fopen(SOLVES_FILE, "a");
    if (!f) {
      printf("ERROR: Could not open solves.csv: %s\n", strerror(errno));
      return;
    }
  } else {
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size == 0) fprintf(f, "timestamp,scramble,time,plus2,dnf,mode\n");
  }

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char ts[20];
  strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", t);

  fprintf(
    f,
    "%s,%s,%.3f,%d,%d,%s\n",
    ts,
    scramble,
    solve_time,
    plus2 ? 1 : 0,
    dnf ? 1 : 0,
    mode
  );
  fclose(f);
}