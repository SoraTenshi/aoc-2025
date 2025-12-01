#include "common.h"

typedef struct {
  s32 pos;
  u32 clicks;
} dial_result;

s64 rotate(s32 original, s32 direction) {
  return ((original + direction) % 100 + 100) % 100;
}

void rotate2(ref dial_result *original, s32 direction) {
  s32 final_pos = original->pos + direction;
  if(final_pos >= 100) {
    s32 add = final_pos / 100;
    original->clicks += add;
  } else if (final_pos <= 0) {
    s32 add = -final_pos / 100 + (s32)(original->pos != 0);
    original->clicks += add;
  }

  original->pos = rotate(final_pos, 0);
}

int main(void) {
  const string input = read_file("input.txt");
  //const string input = read_file("example.txt");
  if(input.data == NULL || input.len == 0) {
    printf("Failed to read input.\n");
    return 1;
  }

  ssize result = 0;
  s32 pos = 50;
  dial_result result2 = {.pos = 50, .clicks = 0};
  slice token = {0};
  slice sub_slice = {0};

  str_split_foreach(input, '\n', token) {
    s32 num = {0};
    if(!slice_new(&token, 1, token.len, &sub_slice)) {
      printf("invalid slice: %.*s\n", token.len, (u8*)token.data);
      return 1;
    }
    num = slice_to_int(sub_slice, s32) * (*(const u8*)token.data == 'L' ? -1 : 1);
    pos = rotate(pos, num);
    rotate2(&result2, num);

    if(pos == 0) ++result;
  }

  printf("Result: %d\n", result);
  printf("Result2: %d\n", result2.clicks);

  // yeah there is memory that "could" be cleaned up, but why should i do it
  // when the os already does that for me? :)
  return 0;
}
