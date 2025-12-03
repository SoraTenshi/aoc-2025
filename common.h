#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef uint8_t           u8;
typedef uint16_t          u16;
typedef uint32_t          u32;
typedef uint64_t          u64;
typedef unsigned __int128 u128;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef __int128 s128;

typedef size_t    usize;
typedef ptrdiff_t ssize;

typedef uintptr_t ptr;

typedef float  f32;
typedef double f64;

typedef u32 rune;
typedef u8  byte;

#define global_var       static
#define local_persistent static
#define internal         static

#define own
#define ref
#define move

typedef u8 bool;
#define true  1
#define false 0

#if defined(_MSC_VER)
#define force_inline __forceinline
#else
#define force_inline __attribute__((always_inline)) inline
#endif

#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#define array_len(x) (sizeof(x) / sizeof((x)[0]))

#if defined(_MSC_VER)
#define aligned(x) __declspec(align(x))
#else
#define aligned(x) __attribute__((aligned(x)))
#endif

typedef struct {
  const u8 *data;
  usize len;
} string;

#define MAX_STRING_LEN (1ULL << 32)
#define s(literal) ((string){ .data = (const u8*)(literal), .len = sizeof(literal) - 1})
#define str_from_cstr_n(s, max_len) ((string){ .data = (const u8*)(s), .len = strnlen(s, max_len)})
#define str_from_cstr(s) str_from_cstr_n(s, MAX_STRING_LEN)

internal bool str_eq(string lhs, string rhs) {
  return lhs.len == rhs.len && memcmp(lhs.data, rhs.data, lhs.len) == 0;
}

internal bool str_starts_with(string s, string prefix) {
  return s.len >= prefix.len && memcmp(s.data, prefix.data, prefix.len) == 0;
}

internal bool str_ends_with(string s, string suffix) {
  return s.len >= suffix.len && memcmp(s.data + s.len - suffix.len, suffix.data, suffix.len) == 0;
}

typedef struct {
  const void *data;
  usize len;
} slice;

#define slice_get(s, T, i) (((const T*)(s).data)[i])
#define slice_len(s, T) ((s).len / sizeof(T))

internal bool slice_new(ref const slice *s, usize start, usize end, own slice *out) {
  if(start > s->len || end > s->len || start >= end) {
    return false;
  }
  out->data = (const void*)(const u8*)(s->data + start);
  out->len = end;

  return true;
}

typedef struct {
  const u8 *current;
  const u8 *end;
} str_split_iter;

internal str_split_iter str_split_begin(ref const string *s) {
  return (str_split_iter){ .current = s->data, .end = s->data + s->len };
}

internal bool str_split_next(move str_split_iter *it, const u8 delim, move slice *out) {
  if(it->current >= it->end) return false;

  const u8 *start = it->current;
  for(const u8 *p = start; p < it->end; ++p) {
    if (*p == delim) {
      out->data = start;
      out->len = p - start;
      it->current = p + 1;
      return true;
    }
  }

  out->data = start;
  out->len = it->end - start;
  it->current = it->end;
  return true;
}

internal bool str_split_next_str(move str_split_iter *it, u8 delim, move string *out) {
  return str_split_next(it, delim, (slice *)out);
}

#define str_split_foreach(s, delim, token) \
  for (str_split_iter _it = str_split_begin(&(s)); \
       str_split_next(&_it, delim, (slice*)&(token));)

/* parse from file */
internal own string read_file(ref const u8 *path) {
  FILE *f = fopen(path, "rb");
  if(f == NULL) return (string){0};

  fseek(f, 0, SEEK_END);
  usize size = (usize)ftell(f);
  fseek(f, 0, SEEK_SET);

  u8 *data = malloc(size + 1);
  if(data == NULL) {
    fclose(f);
    return (string){0};
  }

  fread(data, 1, size, f);
  data[size] = 0;
  fclose(f);

  return (string){.data = data, .len = size};
}

internal s64 slice_to_s64(slice s) {
    s64 result = 0;
    bool negative = false;
    usize i = 0;

    const u8 *data = (const u8*)s.data;

    if (i < s.len && data[i] == '-') {
        negative = true;
        ++i;
    } else if (i < s.len && data[i] == '+') {
        ++i;
    }

    while (i < s.len && data[i] >= '0' && data[i] <= '9') {
        result = result * 10 + (data[i] - '0');
        ++i;
    }

    return negative ? -result : result;
}

internal u64 slice_to_u64(slice s) {
    u64 result = 0;
    usize i = 0;

    const u8 *data = (const u8*)s.data;

    while (i < s.len && data[i] >= '0' && data[i] <= '9') {
        result = result * 10 + (data[i] - '0');
        ++i;
    }

    return result;
}

#define slice_to_int(s, T)   (T)slice_to_s64(s)
#define slice_to_uint(s, T)  (T)slice_to_u64(s)

internal void println_u128(const char *label, u128 n) {
  printf("%s", label);

  if(n == 0) {
    printf("0");
    return;
  }

  char buf[40] = {0};
  int pos = 39;
  buf[pos] = '\0';

  while(n > 0) {
    buf[--pos] = '0' + (n % 10);
    n /= 10;
  }

  printf("%s\n", &buf[pos]);
}

#endif /* COMMON_H_ */
