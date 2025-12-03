#include "common.h"

typedef struct {
  u8 first;
  u8 second;
} joltage;

u64 total_joltage(const slice *s) {
  joltage j = {0};
  usize stop_marker = 1;

  j.first = ((u8*)s->data)[0] - '0';
  for(s32 i = 1; i < s->len - 1; ++i) {
    u8 this = ((u8*)s->data)[i] - '0';
    if(this > j.first
       && i < (s->len - 1)) {
      j.first = this;
      stop_marker = i + 1;
    }
  }

  for(s32 i = stop_marker; i < s->len; ++i) {
    u8 this = ((u8*)s->data)[i] - '0';
    if(this > j.second) {
      j.second = this;
    }
  }

  return j.first * 10 + j.second;
}

u128 total_joltage2(const slice *s) {
  usize max_left = 12;
  u128 res = 0;
  usize stop_marker = 0;

  for(s32 x = 0; x < max_left; ++x) {
    usize remaining = max_left - x;
    usize window_end = s->len - remaining + 1;

    u8 max_digit = ((u8*)s->data)[stop_marker] - '0';
    usize max_stop = stop_marker;

    for(s32 y = stop_marker + 1; y < window_end; ++y) {
      u8 this = ((u8*)s->data)[y] - '0';
      if(this > max_digit) {
        max_digit = this;
        max_stop = y;
      }
    }

    res = res * 10 + max_digit;
    stop_marker = max_stop + 1;
  }

  return res;
}

int main() {
  //const string input = read_file("example.txt");
  const string input = read_file("input.txt");

  if(!str_valid(input)) {
    printf("Failed to parse file\n");
    return 1;
  }

  slice bank = {0};
  u64 res = 0;
  u128 res2 = 0;

  str_split_foreach(input, '\n', bank) {
    res += total_joltage(&bank);
    res2 += total_joltage2(&bank);
  }
  printf("Result: %d\n", res);
  println_u128("Result2: ", res2);
  return 0;
}
