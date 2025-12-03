#include "common.h"

u128 fake_id(u128 start, const u128 end) {
  u128 sum = 0;
  for(u128 num = start; num <= end; ++num) {
    s32 digit_count = 0;
    for(u128 i = num; i > 0; i /= 10) ++digit_count;
    if(digit_count % 2 != 0) continue;

    u64 div = 1;
    for(s32 i = 0; i < digit_count / 2; ++i) div *= 10;

    u128 first_half = num / div;
    u128 scnd_half  = num % div;

    if(first_half == scnd_half) sum += num;
  }

  return sum;
}

u128 fake_id2(u128 start, const u128 end) {
  u128 sum = 0;
  for(u128 num = start; num <= end; ++num) {
    u8 buf[40] = {0};
    usize len = 0;
    u128 temp = num;

    while (temp > 0) {
      buf[len++] = '0' + (temp % 10);
      temp /= 10;
    }

    for (s32 i = 0; i < len / 2; ++i) {
      u8 tmp = buf[i];
      buf[i] = buf[len - 1 - i];
      buf[len - 1 - i] = tmp;
    }

    string digits = {.data = buf, .len = len};

    for (usize length = 1; length <= digits.len / 2; ++length) {
      if (digits.len % length != 0)
        continue;

      usize repeat = digits.len / length;
      if (repeat < 2)
        continue;

      bool pattern = true;
      for (usize i = length; i < digits.len; ++i) {
        if (digits.data[i] != digits.data[i % length]) {
          pattern = false;
          break;
        }
      }

      if (pattern) {
        sum += num;
        break;
      }
    }
  }

  return sum;
}

int main(void) {
  const string input = read_file("input.txt");
  //const string input = read_file("example.txt");
  if(input.data == NULL || input.len == 0) {
    printf("Failed to read input.\n");
    return 1;
  }

  u128 fake_ids = 0;
  u128 fake_ids2 = 0;

  slice id_pair = {0};
  slice id = {0};

  str_split_foreach(input, ',', id_pair) {
    u64 start = 0, end = 0;
    str_split_foreach(id_pair, '-', id) {
      if(start == 0) {
        start = slice_to_uint(id, u128);
      } else {
        end = slice_to_uint(id, u128);
      }
    }
    fake_ids += fake_id(start, end);
    fake_ids2 += fake_id2(start, end);
    start = 0, end = 0;
  }
  println_u128("Result1: ", fake_ids);
  println_u128("Result2: ", fake_ids2);
  return 0;
}
