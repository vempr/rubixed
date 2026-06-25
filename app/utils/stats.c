#include "raylib.h"
#include "stdio.h"
#include "../app.h"
#include "stats.h"
#include "table.h"
#include "../theme.h"
#include "../../ui/ui.h"
#include "../../storage/log.h"

static void reload_solves(StatsTabCtx *s) {
  s->count = load_solves(s->solves, s->app->mode);
  if (s->count <= 0) {
    s->app->isInDialogView = 0;
    s->current = 0;
    return;
  } else if (s->current >= s->count) {
    s->current = s->count - 1;
  }
}

static void update_solve_in_file(int solveNumber, int togglePlus2, int deleteSolve) {
  FILE *f = fopen(SOLVES_FILE, "r");
  if (!f) return;

  int lineCount = 0;
  char buffer[512];

  while (fgets(buffer, sizeof(buffer), f)) {
    lineCount++;
  }

  if (lineCount < 2) {
    fclose(f);
    return;
  }

  // char **lines = malloc(lineCount * sizeof(char *));
  // if (!lines) {
  //   fclose(f);
  //   return;
  // }

  // while (fgets(lines[lineCount], sizeof(lines[0]), f) && lineCount < 1000) {
  //   lineCount++;
  // }
  // fclose(f);

  // for (int i = 0; i < lineCount; i++) {
  //   lines[i] = malloc(512);
  //   if (!lines[i]) {
  //     for (int j = 0; j < i; j++) {
  //       free(lines[j]);
  //     }
  //     free(lines);
  //     fclose(f);
  //     return;
  //   }
  //   fgets(lines[i], 512, f);
  // }
  // fclose(f);

  int targetLine = solveNumber;
  if (targetLine < 1 || targetLine >= lineCount) {
    fclose(f);
    return;
  }

  rewind(f);

  FILE *temp = fopen("solves_temp.csv", "w");
  if (!temp) {
    fclose(f);
    return;
  }

  int currentLine = 0;

  while (fgets(buffer, sizeof(buffer), f)) {
    if (deleteSolve && currentLine == targetLine) {
      currentLine++;
      continue;
    }

    if (togglePlus2 && currentLine == targetLine) {
      buffer[strcspn(buffer, "\n")] = 0;

      char *lineCopy = strdup(buffer);
      char *token;
      char *fields[6];
      int fieldCount = 0;

      token = strtok(lineCopy, ",");
      while (token && fieldCount < 6) {
        fields[fieldCount++] = token;
        token = strtok(NULL, ",\n");
      }

      if (fieldCount >= 4) {
        int currentPlus2 = atoi(fields[3]);
        fprintf(
          temp,
          "%s,%s,%s,%d,%s,%s\n",
          fields[0],
          fields[1],
          fields[2],
          currentPlus2 ? 0 : 1,
          fields[4],
          fields[5]
        );
      } else {
        fprintf(temp, "%s\n", buffer);
      }

      free(lineCopy);
    } else {
      fputs(buffer, temp);
    }

    currentLine++;
  }

  fclose(f);
  fclose(temp);

  remove(SOLVES_FILE);
  rename("solves_temp.csv", SOLVES_FILE);

  // if (deleteSolve) {
  //   free(lines[targetLine]);

  //   for (int i = targetLine; i < lineCount - 1; i++) {
  //     strcpy(lines[i], lines[i + 1]);
  //   }
  //   lineCount--;
  // } else if (togglePlus2) {
  //   // timestamp,scramble,time,plus2,dnf,mode
  //   char *line = strdup(lines[targetLine]);
  //   char *token;
  //   char *fields[6];
  //   int fieldCount = 0;

  //   token = strtok(line, ",");
  //   while (token && fieldCount < 6) {
  //     fields[fieldCount++] = token;
  //     token = strtok(NULL, ",\n");
  //   }

  //   if (fieldCount >= 4) {
  //     int currentPlus2 = atoi(fields[3]);
  //     snprintf(
  //       lines[targetLine], 512,
  //       "%s,%s,%s,%d,%d,%s\n",
  //       fields[0],
  //       fields[1],
  //       fields[2],
  //       currentPlus2 ? 0 : 1,
  //       fields[4],
  //       fields[5]
  //     );
  //   }

  //   free(line);
  // }

  // f = fopen(SOLVES_FILE, "w");
  // if (f) {
  //   for (int i = 0; i < lineCount; i++) {
  //     fputs(lines[i], f);
  //     free(lines[i]);
  //   }
  //   fclose(f);
  // } else {
  //   for (int j = 0; j < lineCount; j++) {
  //     free(lines[j]);
  //   }
  // }

  // free(lines);
}

void draw_time_tab(void *ctx, Rectangle area) {
  int fontSize = 20 * ui_scale();
  int yShift = 0;

  StatsTabCtx *s = ctx;
  if (s->count <= 0) return;

  SolveEntry *solves = s->solves;
  int i = s->current;
  SolveEntry currentSolve = solves[i];

  float time = currentSolve.plus2 ? currentSolve.time + 2.0 : currentSolve.time;
  const char* tt = currentSolve.dnf ?
    TextFormat("%.2f (DNF)", time) :
    TextFormat("%.2f", time);
  
  yShift += 120;

  DrawText(
    currentSolve.timestamp,
    area.x,
    area.y + yShift,
    fontSize - 8,
    Fade(COLOR_TEXT, 0.8)
  );

  yShift += 50;

  DrawText(
    tt,
    area.x,
    area.y + yShift,
    fontSize + 15,
    currentSolve.dnf ? COLOR_DESTRUCTIVE : COLOR_TEXT
  );

  DrawText(
    currentSolve.plus2 ? "(+2 Penalty)" : "(No +2)",
    area.x + MeasureText(tt, fontSize) + 100,
    area.y + yShift + 20,
    fontSize - 10,
    currentSolve.plus2 ? ORANGE : COLOR_TEXT
  );

  yShift += 70;

  DrawText(
    TextFormat("Scramble: %s", currentSolve.scramble),
    area.x,
    area.y + yShift,
    fontSize - 5,
    COLOR_TEXT
  );

  yShift += 60;

  if (s->copied && (GetTime() - s->copiedTime > 1.0)) {
    s->copied = 0;
  }

  if (ui_button((Rectangle){area.x, area.y + yShift, 300, 60}, s->copied ? "Solve details copied!" : "Copy solve details", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false) && !s->copied) {
    s->copied = 1;
    s->copiedTime = GetTime();

    const char* clipboardText;

    if (currentSolve.dnf) {
      clipboardText = TextFormat(
        "-- Generated by vempr/rubixed --\n\n"
        "Date: %s\n"
        "Time: %.2f (DNF)\n"
        "Penalty: %s\n"
        "Scramble: %s\n",
        time,
        currentSolve.plus2 ? "+2" : "None",
        currentSolve.scramble,
        currentSolve.timestamp
      );
    } else {
      clipboardText = TextFormat(
        "-- Generated by vempr/rubixed --\n\n"
        "Date: %s\n"
        "Time: %.2f\n"
        "Penalty: %s\n"
        "Scramble: %s\n",
        currentSolve.timestamp,
        time,
        currentSolve.plus2 ? "+2" : "None",
        currentSolve.scramble
      );
    }

    SetClipboardText(clipboardText);
  }

  yShift += 80;

  if (ui_button((Rectangle){area.x, area.y + yShift, 200, 50}, currentSolve.plus2 ? "Remove +2" : "Penalize +2", COLOR_BG, currentSolve.plus2 ? ORANGE : COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false)) {
    update_solve_in_file(i + 1, 1, 0);
    reload_solves(s);
    s->copied = 0;
  }

  if (ui_button((Rectangle){area.x + 200, area.y + yShift, 200, 50}, "Delete solve", COLOR_DESTRUCTIVE, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false)) {
    update_solve_in_file(i + 1, 0, 1);
    reload_solves(s);
    s->copied = 0;
    s->app->isInDialogView = 0;
  }
}

void draw_ao5_tab(void *ctx, Rectangle area) {
  int fontSize = 20 * ui_scale();
  int yShift = 0;

  StatsTabCtx *s = ctx;
  SolveEntry *solves = s->solves;
  int i = s->current;
  SolveEntry lastSolve = solves[i];

  float ao5 = compute_average(solves, i, 5);

  char ao5Buffer[32];
  if (ao5 < -1.5f) strcpy(ao5Buffer, "Not enough solves");
  else if (ao5 < -0.5f) strcpy(ao5Buffer, "DNF");
  else snprintf(ao5Buffer, sizeof(ao5Buffer), "%.2f", ao5);
  
  yShift += 120;

  DrawText(
    TextFormat("Last solve at: %s", lastSolve.timestamp),
    area.x,
    area.y + yShift,
    fontSize - 8,
    Fade(COLOR_TEXT, 0.8)
  );

  yShift += 50;

  Color ao5Color = COLOR_TEXT;
  if (ao5 < -0.5f && ao5 > -1.5f) ao5Color = COLOR_DESTRUCTIVE;

  DrawText(
    TextFormat("AO5: %s", ao5Buffer),
    area.x,
    area.y + yShift,
    fontSize + 15,
    ao5Color
  );

  yShift += 70;

  float times[5];
  int indices[5];
  int best = -1, worst = -1;

  if (ao5 >= 0.0f || (ao5 < -0.5f && ao5 > -1.5f)) {
    for (int j = 0; j < 5; j++) {
      int idx = i - (4 - j);
      indices[j] = idx;

      if (solves[idx].dnf) {
        times[j] = __FLT_MAX__;
      } else {
        times[j] = solves[idx].plus2 ? solves[idx].time + 2.0f : solves[idx].time;
      }
    }

    float bestTime = __FLT_MAX__, worstTime = -__FLT_MAX__;

    for (int j = 0; j < 5; j++) {
      if (times[j] == __FLT_MAX__) continue;
      if (times[j] < bestTime) {
        bestTime = times[j]; best = j;
      } 
      if (times[j] > worstTime) {
        worstTime = times[j]; worst = j;
      }
    }

    char display[256] = "";
    char temp[32];

    for (int j = 0; j < 5; j++) {
      if (solves[indices[j]].dnf) {
        snprintf(temp, sizeof(temp), "DNF", times[j]);
      } else {
        if (j == best || j == worst) {
          snprintf(temp, sizeof(temp), "(%.2f)", times[j]);
        } else {
          snprintf(temp, sizeof(temp), "%.2f", times[j]);
        }
      }

      if (j > 0) strcat(display, "  ");
      strcat(display, temp);
    }

    DrawText(
      display,
      area.x,
      area.y + yShift,
      fontSize - 5,
      COLOR_TEXT
    );

    yShift += 60;
  }

  if (s->copied && (GetTime() - s->copiedTime > 1.0)) {
    s->copied = 0;
  }

  if (ui_button((Rectangle){area.x, area.y + yShift, 300, 60}, s->copied ? "Ao5 details copied!" : "Copy Ao5 details", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false) && !s->copied) {
    s->copied = 1;
    s->copiedTime = GetTime();

    if (ao5 < -1.5f) {
      const char* clipboardText = TextFormat(
        "-- Generated by vempr/rubixed --\n\n"
        "AO5: %s\n\n"
        "Not enough solves to calculate average.\n",
        ao5Buffer
      );

      SetClipboardText(clipboardText);

      return;
    }

    char temp[64];
    char headerLine[512] = "";
    char allSolves[4096] = "";

    for (int j = 0; j < 5; j++) {
      if (solves[indices[j]].dnf) {
        snprintf(temp, sizeof(temp), "DNF", times[j]);
      } else {
        if (j == best || j == worst) {
          snprintf(temp, sizeof(temp), "(%.2f)", times[j]);
        } else {
          snprintf(temp, sizeof(temp), "%.2f", times[j]);
        }
      }

      if (j > 0) strcat(headerLine, "  ");
      strcat(headerLine, temp);
    }

    for (int j = 0; j < 5; j++) {
      float time = solves[indices[j]].plus2 ? solves[indices[j]].time + 2.0 : solves[indices[j]].time;

      char solveDetail[512];
      if (solves[indices[j]].dnf) {
        snprintf(solveDetail, sizeof(solveDetail),
          "Date: %s\n"
          "Time: %.2f (DNF)\n"
          "Penalty: %s\n"
          "Scramble: %s\n",
          solves[indices[j]].timestamp,
          time,
          solves[indices[j]].plus2 ? "+2" : "None",
          solves[indices[j]].scramble
        );
      } else {
        snprintf(solveDetail, sizeof(solveDetail),
          "Date: %s\n"
          "Time: %.2f\n"
          "Penalty: %s\n"
          "Scramble: %s\n",
          solves[indices[j]].timestamp,
          time,
          solves[indices[j]].plus2 ? "+2" : "None",
          solves[indices[j]].scramble
        );
      }

      strcat(allSolves, solveDetail);
      if (j < 4) strcat(allSolves, "\n");
    }

    const char* clipboardText = TextFormat(
      "-- Generated by vempr/rubixed --\n\n"
      "%s\n"
      "AO5: %s\n\n"
      "%s",
      headerLine,
      ao5Buffer,
      allSolves
    );

    SetClipboardText(clipboardText);
  }
}

void draw_ao12_tab(void *ctx, Rectangle area) {
  int fontSize = 20 * ui_scale();
  int yShift = 0;

  StatsTabCtx *s = ctx;
  SolveEntry *solves = s->solves;
  int i = s->current;
  SolveEntry lastSolve = solves[i];

  float ao12 = compute_average(solves, i, 12);

  char ao12Buffer[32];
  if (ao12 < -1.5f) strcpy(ao12Buffer, "Not enough solves");
  else if (ao12 < -0.5f) strcpy(ao12Buffer, "DNF");
  else snprintf(ao12Buffer, sizeof(ao12Buffer), "%.2f", ao12);
  
  yShift += 120;

  DrawText(
    TextFormat("Last solve at: %s", lastSolve.timestamp),
    area.x,
    area.y + yShift,
    fontSize - 8,
    Fade(COLOR_TEXT, 0.8)
  );

  yShift += 50;

  Color ao12Color = COLOR_TEXT;
  if (ao12 < -0.5f && ao12 > -1.5f) ao12Color = COLOR_DESTRUCTIVE;

  DrawText(
    TextFormat("AO12: %s", ao12Buffer),
    area.x,
    area.y + yShift,
    fontSize + 15,
    ao12Color
  );

  yShift += 70;

  float times[12];
  int indices[12];
  int best = -1, worst = -1;

  if (ao12 >= 0.0f || (ao12 < -0.5f && ao12 > -1.5f)) {
    for (int j = 0; j < 12; j++) {
      int idx = i - (11 - j);
      indices[j] = idx;

      if (solves[idx].dnf) {
        times[j] = __FLT_MAX__;
      } else {
        times[j] = solves[idx].plus2 ? solves[idx].time + 2.0f : solves[idx].time;
      }
    }

    float bestTime = __FLT_MAX__, worstTime = -__FLT_MAX__;

    for (int j = 0; j < 12; j++) {
      if (times[j] == __FLT_MAX__) continue;
      if (times[j] < bestTime) {
        bestTime = times[j]; best = j;
      } 
      if (times[j] > worstTime) {
        worstTime = times[j]; worst = j;
      }
    }

    char display[256] = "";
    char temp[32];

    for (int j = 0; j < 12; j++) {
      if (solves[indices[j]].dnf) {
        snprintf(temp, sizeof(temp), "DNF", times[j]);
      } else {
        if (j == best || j == worst) {
          snprintf(temp, sizeof(temp), "(%.2f)", times[j]);
        } else {
          snprintf(temp, sizeof(temp), "%.2f", times[j]);
        }
      }

      if (j > 0) strcat(display, "  ");
      strcat(display, temp);
    }

    DrawText(
      display,
      area.x,
      area.y + yShift,
      fontSize - 5,
      COLOR_TEXT
    );

    yShift += 60;
  }

  if (s->copied && (GetTime() - s->copiedTime > 1.0)) {
    s->copied = 0;
  }

  if (ui_button((Rectangle){area.x, area.y + yShift, 300, 60}, s->copied ? "Ao12 details copied!" : "Copy Ao12 details", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false) && !s->copied) {
    s->copied = 1;
    s->copiedTime = GetTime();

    if (ao12 < -1.5f) {
      const char* clipboardText = TextFormat(
        "-- Generated by vempr/rubixed --\n\n"
        "AO12: %s\n\n"
        "Not enough solves to calculate average.\n",
        ao12Buffer
      );

      SetClipboardText(clipboardText);

      return;
    }

    char temp[64];
    char headerLine[512] = "";
    char allSolves[4096] = "";

    for (int j = 0; j < 12; j++) {
      if (solves[indices[j]].dnf) {
        snprintf(temp, sizeof(temp), "DNF", times[j]);
      } else {
        if (j == best || j == worst) {
          snprintf(temp, sizeof(temp), "(%.2f)", times[j]);
        } else {
          snprintf(temp, sizeof(temp), "%.2f", times[j]);
        }
      }

      if (j > 0) strcat(headerLine, "  ");
      strcat(headerLine, temp);
    }

    for (int j = 0; j < 12; j++) {
      float time = solves[indices[j]].plus2 ? solves[indices[j]].time + 2.0 : solves[indices[j]].time;

      char solveDetail[512];
      if (solves[indices[j]].dnf) {
        snprintf(solveDetail, sizeof(solveDetail),
          "Date: %s\n"
          "Time: %.2f (DNF)\n"
          "Penalty: %s\n"
          "Scramble: %s\n",
          solves[indices[j]].timestamp,
          time,
          solves[indices[j]].plus2 ? "+2" : "None",
          solves[indices[j]].scramble
        );
      } else {
        snprintf(solveDetail, sizeof(solveDetail),
          "Date: %s\n"
          "Time: %.2f\n"
          "Penalty: %s\n"
          "Scramble: %s\n",
          solves[indices[j]].timestamp,
          time,
          solves[indices[j]].plus2 ? "+2" : "None",
          solves[indices[j]].scramble
        );
      }

      strcat(allSolves, solveDetail);
      if (j < 11) strcat(allSolves, "\n");
    }

    const char* clipboardText = TextFormat(
      "-- Generated by vempr/rubixed --\n\n"
      "%s\n"
      "AO12: %s\n\n"
      "%s",
      headerLine,
      ao12Buffer,
      allSolves
    );

    SetClipboardText(clipboardText);
  }
}