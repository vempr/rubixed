#ifndef SCRAMBLE_H
#define SCRAMBLE_H

void scramble_init(const char* cache_dir);
char* generate_scramble(int max_moves);
char* invert_scramble(const char* scramble);

#endif