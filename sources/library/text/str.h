#pragma once

#include <library/base/macros.h>
#include <library/base/std.h>

struct str
{
    char *buffer;
    size_t len;
};

typedef struct str str_t;

#define str_fix_t(N)    \
    struct              \
    {                   \
        char buffer[N]; \
        size_t len;     \
    }

typedef str_fix_t(8) str_fix8_t;
typedef str_fix_t(16) str_fix16_t;
typedef str_fix_t(32) str_fix32_t;
typedef str_fix_t(64) str_fix64_t;
typedef str_fix_t(128) str_fix128_t;

static inline size_t cstr_len(char const *str)
{
    size_t size = 0;

    while (str[size])
    {
        size++;
    }

    return size;
}

static inline str_t str_forward(str_t str)
{
    return str;
}

static inline str_t str_make_from_cstr(char const *cstr)
{
    return (str_t){(char *)cstr, cstr_len(cstr)};
}

// clang-format off

#define make_str(literal)                \
    _Generic((literal),                  \
        str_t: str_forward,              \
        char*: str_make_from_cstr,       \
        char const*: str_make_from_cstr  \
    )(literal)

#define make_str_n(str, n) \
    (str_t){str, n}

// clang-format on

void str_rvs(str_t str);

bool str_eq(str_t lhs, str_t rhs);
