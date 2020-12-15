#pragma once

#include <stdint.h>

#define KIBIBYTES(n) (n*1024)
#define MEBIBYTES(n) (n*1024*KIBIBYTES(1))
#define GIBIBYTES(n) (n*1024*MEBIBYTES(1))

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float	 r32;
typedef double	 r64;

typedef r64 real;

typedef u8 bool;
enum {false, true};

#define ASSERT(x) if(!(x)) *(r32*)(0) = 1
#define INVALID_CODE_PATH ASSERT(0)

#define ABS(x)    ((x<0)?-x:x)
#define MAX(x, y) ((x>y)?x:y)
#define MIN(x, y) ((x<y)?x:y)
#define CLAMP(x, l, h) MAX(MIN(x,h), l)

#define Alloc(type) malloc(sizeof(type))
#define AllocArray(type, n) malloc(sizeof(type)*n)
#define Free(a) free(a)

